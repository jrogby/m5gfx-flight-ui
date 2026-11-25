#pragma once
#include <stdlib.h>  // rand(), srand()
#include <string.h>
#include <vector>
#include <functional>
#include <ReactESP.h>
#include <M5Unified.hpp>

#include <flightui/Touch.h>
#include <flightui/types.h>
#include <flightui/interfaces.h>
#include <flightui/color.h>
#include <flightui/sfx.h>
#include <flightui/fonts/slots.h>
#include <flightui/screensavers/InProgress.h>
#include <flightui/screensavers/Shapes.h>
#include <flightui/screensavers/Shapes2.h>
#include <flightui/screensavers/Meltable.h>
/**
 * Page lifecycle, touch navigation, screensaver. No app globals: the app adds pages (add_page),
 * passes start page to begin(), screensaver via apply_screensaver(), and persists the page in on_page_settled.
 */

class DisplayManager;
extern DisplayManager display_manager;

class DisplayManager {
 public:
  DisplayManager() = default;

  void enable_draw_time_measurement(bool enable) {
    measure_draw_time = enable;
    if (!enable) {
      total_draw_time = 0;
      draw_count = 0;
    }
  }

  // canvas: full-screen page buffer (pushed to lcd). start_page: first page shown (e.g. from NVS), 0 if out of range
  void begin(reactesp::EventLoop& event_loop, M5Canvas& canvas, lgfx::LovyanGFX& lcd, uint8_t start_page = 0) {
    _event_loop = &event_loop;
    _buf = &canvas;
    _lcd = &lcd;
    flightui::font::warn_fallback_slots();
    _start_page = start_page;
    _settled_page = start_page;
    _event_loop->onDelay(0, [this]() { setup(); });
  }

  /**
   * Add a new page to the display manager.
   * The page will be managed (setup, update, kill) by the display manager and included in page navigation.
   * Pages should be added before calling begin() to ensure they are included in initial setup and navigation.
   */
  void add_page(IScreenPage* page) {
    page_collection.push_back(page);
  }

  /**
   * Remove a page from the display manager. The page will no longer be managed or included in navigation.
   * Note: This does not delete the page object itself, so the caller is responsible for managing the page's memory if needed.
   */
  void remove_page(IScreenPage* page) {
    page_collection.erase(
        std::remove(page_collection.begin(), page_collection.end(), page),
        page_collection.end());
  }

  /**
   * Clear all pages from the display manager. After calling this, no pages will be managed or included in navigation.
   * Note: This does not delete the page objects themselves, so the caller is responsible for managing the pages' memory if needed.
   */
  size_t page_count() const { return page_collection.size(); }
  void clear_pages() {
    page_collection.clear();
  }

  // Called once a page has stayed selected for PAGE_SAVE_DELAY_MS (persist it: NVS write = flash wear)
  void on_page_settled(std::function<void(uint8_t)> cb) { _on_page_settled = std::move(cb); }

 // Re-create the screensaver. type: "shapes", "meltable", "inprogress", "none"; timeout_s 0 = disabled. App loop task only.
  void apply_screensaver(const char* type, uint16_t timeout_s) {
    strlcpy(_ss_type, type, sizeof(_ss_type));
    _idle_timeout_ms = (ulong)timeout_s * 1000UL;
    if (screensaver && screensaver->is_active()) {
      screensaver->stop();
      init_screensaver();
      select_page(current_page);  // redraw the page the screensaver covered
    } else {
      init_screensaver();
    }
  }

 private:
  reactesp::EventLoop* _event_loop = nullptr;
  ulong _idle_timeout_ms = 0; // 0 = screensaver disabled
  ulong _read_input_millis = 10;
  ulong _fast_millis = 33; // screensaver, animations etc.
  ulong _slow_millis = 1000; // info tables etc.
  //static constexpr uchar num_pages = 15;
  //uchar last_page = num_pages - 1;
  //uchar first_page = 0;
  //uchar previous_page = 0;
  Touch touch;
  std::vector<IScreenPage*> page_collection;  // Dynamic container for pages
  IScreenPage* selected_page = nullptr;
  uchar current_page = 0;
  std::vector<bool> _page_alive;  // page has been set up and not killed (sliding window: prev, current, next)
  ulong last_touch = 0;
  ulong last_page_update = 0;
  uint8_t _start_page = 0;
  uint8_t _settled_page = 0;    // last page reported via _on_page_settled
  ulong _page_changed_at = 0;   // 0 = nothing pending
  std::function<void(uint8_t)> _on_page_settled;
  char _ss_type[16] = "none";
  static constexpr ulong PAGE_SAVE_DELAY_MS = 10000;  // persist page only once it stays selected (NVS write = flash wear)
  bool measure_draw_time = true;
  ulong total_draw_time = 0;
  ulong draw_count = 0;
  IScreensaver *screensaver = nullptr;
  M5Canvas* _buf = nullptr;
  lgfx::LovyanGFX* _lcd = nullptr;

  void init_screensaver() {
    delete screensaver;
    screensaver = nullptr;
    if (_idle_timeout_ms == 0) return; // disabled
    if (strcmp(_ss_type, "meltable") == 0)
      screensaver = new Meltable();
    else if (strcmp(_ss_type, "inprogress") == 0)
      screensaver = new InProgress();
    else if (strcmp(_ss_type, "shapes") == 0)
      screensaver = new Shapes();
    // "none" or unknown = no screensaver
    if (screensaver && _buf) screensaver->attach(*_buf, *_lcd);
  }


  void setup() {
    init_screensaver();
    _page_alive.assign(page_collection.size(), false);
    if (!page_collection.empty()) {
      uint8_t start_page = _start_page;
      if (start_page >= page_collection.size()) {
        start_page = 0; // Fallback if saved index is out of range
      }
      select_page(start_page);
    }
    last_touch = 0;
    last_page_update = 0;
    _event_loop->onRepeat(_read_input_millis, [this]() {
      update_display();
      handle_touch();
      persist_page_if_settled();
    });
  }

  void init_pages() {
    for (auto* page : page_collection) {
      page->setup();
    }
  }

  void select_page(uchar page_index) {
    if (page_index >= page_collection.size()) return;
    total_draw_time = 0;
    draw_count = 0;
    _lcd->fillScreen(TFT_BLACK);       // Clear actual LCD display
    _buf->fillScreen(TFT_BLACK);    // Clear sprite buffer
    current_page = page_index;
    selected_page = page_collection[page_index];
    cache_indicator();
    _page_alive[page_index] = true;  // update() sets the page up itself if needed
    selected_page->update();
    draw_page_indicator();
    _buf->pushSprite(0, 0);
    // Report page later via persist_page_if_settled() (only if it differs from the last reported)
    _page_changed_at = (page_index != _settled_page) ? (millis() | 1) : 0;  // never 0 while pending
    kill_pages_outside_window();
    // Set up the new neighbours after this frame is shown, so a swipe never waits for setup()
    _event_loop->onDelay(0, [this]() { preload_neighbor_pages(); });
  }

  uchar prev_index(uchar i) const { return i == 0 ? page_collection.size() - 1 : i - 1; }
  uchar next_index(uchar i) const { return (i + 1) % page_collection.size(); }
  bool in_window(uchar i) const { return i == current_page || i == prev_index(current_page) || i == next_index(current_page); }

  // Free every page that is not current or a direct neighbour (keeps at most 3 pages allocated)
  void kill_pages_outside_window() {
    for (uchar i = 0; i < page_collection.size(); i++) {
      if (_page_alive[i] && !in_window(i)) {
        page_collection[i]->kill();
        _page_alive[i] = false;
      }
    }
  }

  // Set up prev/next of the current page (deferred; window re-checked since swipes may have moved on)
  void preload_neighbor_pages() {
    if (page_collection.size() < 2) return;
    for (uchar i : { prev_index(current_page), next_index(current_page) }) {
      if (!_page_alive[i] && in_window(i)) {
        page_collection[i]->setup();
        _page_alive[i] = true;
      }
    }
  }

  // Report the page once it has stayed selected for PAGE_SAVE_DELAY_MS
  void persist_page_if_settled() {
    if (_page_changed_at && millis() - _page_changed_at >= PAGE_SAVE_DELAY_MS) {
      _page_changed_at = 0;
      _settled_page = current_page;
      if (_on_page_settled) _on_page_settled(current_page);
    }
  }

  void update_display() {
    bool ss_active = screensaver && screensaver->is_active();
    ulong page_millis = selected_page && selected_page->update_ms ? selected_page->update_ms : _slow_millis;
    if (!ss_active && (millis() - last_page_update >= page_millis)) {
      draw_selected_page();
      last_page_update = millis();
      _buf->pushSprite(0, 0);
    } else if (ss_active && screensaver->redraw() && (millis() - last_page_update >= _slow_millis)) {
      draw_selected_page();
      screensaver->update(_fast_millis);
      last_page_update = millis();
    } else if (ss_active) {
      screensaver->update(_fast_millis);
    }
  }

  void draw_selected_page() {
    ulong start_time = millis();
    if (selected_page) {
      selected_page->update();
    }
    // Page position indicator — subtle highlight on title separator line (y=23)
    draw_page_indicator();
    if (_idle_timeout_ms > 0 && screensaver && millis() - last_touch >= _idle_timeout_ms) {
      screensaver->start();
    }
    if(measure_draw_time) {
      ulong draw_time = millis() - start_time;
      total_draw_time += draw_time;
      draw_count++;
      ulong avg_draw_time = total_draw_time / draw_count;

      // Pages draw straight into disp_buf and may leave a VLW font / datum set
      _buf->unloadFont();
      _buf->setFont(&fonts::Font0);  // pages may also leave a GFX font set
      _buf->setTextDatum(TL_DATUM);
      _buf->setTextColor(ui::color::AIRBUS_BLUE, ui::color::ECAM_BG);
      _buf->setCursor(284, 0);
      _buf->printf("%lu ms", avg_draw_time);
    }
  }
  // Cached page indicator geometry
  uint16_t _ind_seg_x = 0;
  uint16_t _ind_seg_end = 0;
  bool _ind_visible = false;
  static constexpr uint16_t IND_Y = 23;
  static constexpr uint16_t IND_X0 = 10;
  static constexpr uint16_t IND_X1 = 310;
  static constexpr uint16_t IND_COLOR = lgfx::v1::color565(60, 80, 100);

  void cache_indicator() {
    _ind_visible = (page_collection.size() > 1 && selected_page && selected_page->has_title);
    if (!_ind_visible) return;
    uint16_t total_w = IND_X1 - IND_X0;
    uint16_t seg_w = total_w / page_collection.size();
    _ind_seg_x = IND_X0 + current_page * seg_w;
    _ind_seg_end = (current_page == page_collection.size() - 1) ? IND_X1 : _ind_seg_x + seg_w;
  }

  // Subtle progress indicator overlaid on title separator line (y=23, x=10..310)
  void draw_page_indicator() {
    if (!_ind_visible) return;
    _buf->drawLine(IND_X0, IND_Y, IND_X1, IND_Y, ui::color::ECAM_ELEC_BG);
    _buf->drawLine(_ind_seg_x, IND_Y, _ind_seg_end, IND_Y, IND_COLOR);
  }

  void handle_touch() {
    touch.update_state();
    handle_long_clicks();
    if (screensaver && screensaver->is_active()) {
      if (M5.Touch.getCount()) {
        screensaver->stop();
        last_touch = millis();
        select_page(current_page);
      }
    } else {
      handle_page_swipes();
      if (M5.Touch.getCount()) last_touch = millis();
    }
  }

  // Send an event to the current page; handled = redraw now
  bool dispatch(TouchEvent::Type type) {
    if (!selected_page) return false;
    TouchEvent ev{type, touch.x, touch.y};
    if (type == TouchEvent::SWIPE_UP || type == TouchEvent::SWIPE_DOWN) ev.speed = touch.speed;
    if (!selected_page->on_touch(ev)) return false;
    last_touch = millis();
    draw_selected_page();  // redraw now (same as a page change), not on the next tick
    _buf->pushSprite(0, 0);
    last_page_update = millis();
    return true;
  }

  void handle_page_swipes() {
    if (touch.swiped_up()) dispatch(TouchEvent::SWIPE_UP);
    if (touch.swiped_down()) dispatch(TouchEvent::SWIPE_DOWN);
    if (touch.short_pressed() && dispatch(TouchEvent::TAP)) M5.Speaker.playWav(g_snd_click_select);
    if (touch.swiped_left()) {
      M5.Speaker.playWav(g_snd_click_forward);
      navigate_next_page();
      last_touch = millis();
    } else if (touch.swiped_right()) {
      M5.Speaker.playWav(g_snd_click_back);
      navigate_prev_page();
      last_touch = millis();
    }
  }

  void handle_long_clicks() {
    if (touch.long_pressed()) {
      M5.Speaker.playWav(g_snd_click_select);
      last_touch = millis();
      if (!(screensaver && screensaver->is_active())) dispatch(TouchEvent::LONG_PRESS);
    }
  }

  void navigate_next_page() {
    if (page_collection.empty()) return;
    uchar next_page = (current_page + 1) % page_collection.size();
    select_page(next_page);
  }

  void navigate_prev_page() {
    if (page_collection.empty()) return;
    uchar prev_page = (current_page == 0) ? page_collection.size() - 1 : current_page - 1;
    select_page(prev_page);
  }
};

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_DISPLAY_MANAGER)
inline DisplayManager display_manager;  // C++17 inline: one instance, header may be included by several .cpp
#endif