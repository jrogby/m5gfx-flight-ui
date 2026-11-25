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
 *
 * Navigation (with groups, set_groups() + add_page(page, name, group)):
 * - swipe left/right: next/prev page within the current group (ring)
 * - swipe up/down not handled by the page: next/prev group (last page shown in that group)
 * - tap the title row (y < MENU_ZONE): page menu. Top row = group keys, below = the group's pages
 *   (white < > = go to page, green = current). Tap a page to go there; swipe, long press, tap outside or timeout closes.
 * - title row shows "n/m" top right (position within the group; put the group in the page title,
 *   e.g. "N2K: DATA INSPECTOR"), the indicator line shows the same position.
 * Without groups everything is one ring, as before.
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
  // Add with a short name (menu, persistence) and a group index into set_groups()
  void add_page(IScreenPage* page, const char* name, uint8_t group = 0) {
    page->name = name;
    page->group = group;
    add_page(page);
  }

  // Group names (menu keys, title label), index = IScreenPage::group. Pointers must stay valid (string literals).
  void set_groups(std::initializer_list<const char*> names) { _groups.assign(names); }

  // Page index by name, -1 if not found
  int find_page(const char* name) const {
    if (!name || !*name) return -1;
    for (size_t i = 0; i < page_collection.size(); i++)
      if (!strcmp(page_collection[i]->name, name)) return (int)i;
    return -1;
  }
  const char* page_name(uint8_t i) const { return i < page_collection.size() ? page_collection[i]->name : ""; }
  // Start page for setup() (call before the event loop runs setup, e.g. right after registering pages)
  void set_start_page(uint8_t i) { _start_page = i; _settled_page = i; }

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
  std::vector<const char*> _groups;
  std::vector<uchar> _last_in_group;  // last page shown per group (group switch returns there)
  bool _menu_open = false;
  uint8_t _menu_group = 0;
  ulong _menu_at = 0;
  static constexpr int16_t MENU_ZONE = 24;           // tap above this y opens the menu
  static constexpr ulong MENU_TIMEOUT_MS = 15000;
  static constexpr int16_t MENU_KEY_H = 36, MENU_LIST_Y = 40;
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
    if (_last_in_group.size() < group_count()) _last_in_group.assign(group_count(), 255);
    _last_in_group[group_of(page_index)] = page_index;
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

  // Groups: without set_groups() all pages are group 0 = one ring
  uint8_t group_count() const { return _groups.empty() ? 1 : _groups.size(); }
  uint8_t group_of(uchar i) const { return page_collection[i]->group < group_count() ? page_collection[i]->group : 0; }
  const char* group_name(uint8_t g) const { return g < _groups.size() ? _groups[g] : ""; }
  // Neighbours within the group of page i (ring); i itself if it is alone in its group
  uchar step_in_group(uchar i, int dir) const {
    const size_t n = page_collection.size();
    for (size_t k = 1; k < n; k++) {
      const uchar j = (uchar)((i + n + dir * (int)k) % n);
      if (group_of(j) == group_of(i)) return j;
    }
    return i;
  }
  uchar prev_index(uchar i) const { return step_in_group(i, -1); }
  uchar next_index(uchar i) const { return step_in_group(i, 1); }
  // Position of page i within its group: index (0-based) and count
  void group_pos(uchar i, uint8_t& pos, uint8_t& count) const {
    pos = 0; count = 0;
    for (uchar j = 0; j < page_collection.size(); j++) {
      if (group_of(j) != group_of(i)) continue;
      if (j == i) pos = count;
      count++;
    }
  }
  // First page of group g (or its last shown page), -1 if the group is empty
  int entry_page(uint8_t g) const {
    if (g < _last_in_group.size() && _last_in_group[g] < page_collection.size() && group_of(_last_in_group[g]) == g)
      return _last_in_group[g];
    for (uchar j = 0; j < page_collection.size(); j++) if (group_of(j) == g) return j;
    return -1;
  }
  // Next/prev non-empty group
  void navigate_group(int dir) {
    const uint8_t gc = group_count();
    if (gc < 2 || page_collection.empty()) return;
    const uint8_t g0 = group_of(current_page);
    for (uint8_t k = 1; k < gc; k++) {
      const uint8_t g = (uint8_t)((g0 + gc + dir * k) % gc);
      const int p = entry_page(g);
      if (p >= 0) { select_page((uchar)p); return; }
    }
  }
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
    if (_menu_open) {
      if (millis() - _menu_at >= MENU_TIMEOUT_MS) close_menu();
      return;  // menu is static: drawn on open/change only
    }
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
    draw_group_label();
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
      _buf->setCursor(284, 232);  // bottom right: the title row has the group label
      _buf->printf("%lu ms", avg_draw_time);
    }
  }
  // Cached page indicator geometry
  uint16_t _ind_seg_x = 0;
  uint16_t _ind_seg_end = 0;
  bool _ind_visible = false;
  static constexpr uint16_t IND_Y = 23;
  static constexpr uint16_t IND_X0 = 40;   // = title line (drawers/Ecam.h TITLE_LINE_X0/X1)
  static constexpr uint16_t IND_X1 = 280;
  static constexpr uint16_t IND_COLOR = lgfx::v1::color565(60, 80, 100);

  void cache_indicator() {  // segment = position within the current group
    uint8_t pos, count;
    group_pos(current_page, pos, count);
    _ind_visible = (count > 1 && selected_page && selected_page->has_title);
    if (!_ind_visible) return;
    uint16_t total_w = IND_X1 - IND_X0;
    uint16_t seg_w = total_w / count;
    _ind_seg_x = IND_X0 + pos * seg_w;
    _ind_seg_end = (pos == count - 1) ? IND_X1 : _ind_seg_x + seg_w;
  }

  // "n/m" (position within the group) top right of the title row; the page title carries the group name
  void draw_group_label() {
    if (_groups.empty() || !selected_page || !selected_page->has_title) return;
    uint8_t pos, count;
    group_pos(current_page, pos, count);
    if (count < 2) return;
    char b[12];
    snprintf(b, sizeof(b), "%u/%u", pos + 1, count);
    _buf->loadFont(flightui::font::ECAMFontRegular12);
    _buf->setTextDatum(TR_DATUM);
    _buf->setTextColor(ui::color::NEUTRAL);
    _buf->drawString(b, 316, 5);
    _buf->setTextDatum(TL_DATUM);
  }

  // ---- page menu ----
  // Pages of group g in registration order
  uint8_t group_pages(uint8_t g, uchar* out, uint8_t max) const {
    uint8_t n = 0;
    for (uchar j = 0; j < page_collection.size() && n < max; j++) if (group_of(j) == g) out[n++] = j;
    return n;
  }
  int16_t menu_row_h(uint8_t n) const {
    const uint8_t rows = (n + 1) / 2;
    return rows ? min<int16_t>(44, (240 - MENU_LIST_Y) / rows) : 44;
  }

  void open_menu() {
    _menu_open = true;
    _menu_group = group_of(current_page);
    _menu_at = millis();
    M5.Speaker.playWav(g_snd_click_select);
    draw_menu();
  }
  void close_menu() {
    _menu_open = false;
    last_page_update = 0;  // redraw the page on the next tick
    _lcd->fillScreen(TFT_BLACK);
  }

  void draw_menu() {
    using namespace ui::color;
    _buf->fillScreen(TFT_BLACK);
    // Group keys (like the ECAM control panel page keys): current group green, others white
    const uint8_t gc = group_count();
    const int16_t kw = 320 / gc;
    _buf->loadFont(flightui::font::ECAMFontRegular16);
    _buf->setTextDatum(MC_DATUM);
    for (uint8_t g = 0; g < gc; g++) {
      const bool on = g == _menu_group;
      _buf->drawRect(g * kw + 2, 2, kw - 4, MENU_KEY_H - 4, on ? ACTIVE : ECAM_ELEC_BG);
      _buf->setTextColor(on ? ACTIVE : NEUTRAL);
      _buf->drawString(_groups.empty() ? "PAGES" : group_name(g), g * kw + kw / 2, MENU_KEY_H / 2);
    }
    // Pages: two columns, left "<NAME", right "NAME>" (white = go to page), current page green without caret
    uchar pages[32];
    const uint8_t n = group_pages(_menu_group, pages, 32);
    const int16_t rh = menu_row_h(n);
    _buf->loadFont(flightui::font::ECAMFontRegular18);
    char b[24];
    for (uint8_t i = 0; i < n; i++) {
      const bool right = i & 1;
      const int16_t y = MENU_LIST_Y + (i / 2) * rh + rh / 2;
      const bool cur = pages[i] == current_page;
      const char* nm = page_collection[pages[i]]->name;
      if (cur) snprintf(b, sizeof(b), "%s", nm);
      else snprintf(b, sizeof(b), right ? "%s>" : "<%s", nm);
      _buf->setTextColor(cur ? ACTIVE : NEUTRAL);
      _buf->setTextDatum(right ? MR_DATUM : ML_DATUM);
      _buf->drawString(b, right ? 314 : 6, y);
    }
    _buf->setTextDatum(TL_DATUM);
    _buf->pushSprite(0, 0);
  }

  void handle_menu_touch() {
    if (touch.swiped_left() || touch.swiped_right() || touch.swiped_up() || touch.swiped_down()) {
      close_menu();
      return;
    }
    if (!touch.short_pressed()) return;
    _menu_at = millis();
    const int16_t x = touch.x, y = touch.y;
    if (y < MENU_KEY_H) {  // group key
      const uint8_t g = (uint8_t)min<int>(x / (320 / group_count()), group_count() - 1);
      if (g != _menu_group) { _menu_group = g; M5.Speaker.playWav(g_snd_click_forward); draw_menu(); }
      return;
    }
    uchar pages[32];
    const uint8_t n = group_pages(_menu_group, pages, 32);
    const int16_t rh = menu_row_h(n);
    const int idx = y >= MENU_LIST_Y ? ((y - MENU_LIST_Y) / rh) * 2 + (x >= 160 ? 1 : 0) : -1;
    if (idx < 0 || idx >= n) { close_menu(); return; }  // outside the list
    M5.Speaker.playWav(g_snd_click_select);
    _menu_open = false;
    select_page(pages[idx]);
    last_touch = millis();
  }

  // Subtle progress indicator overlaid on title separator line (y=23, x=40..280)
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
    } else if (_menu_open) {
      handle_menu_touch();
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
    // Up/down: the page first; unhandled = change group
    if (touch.swiped_up() && !dispatch(TouchEvent::SWIPE_UP) && group_count() > 1) {
      M5.Speaker.playWav(g_snd_click_forward);
      navigate_group(1);
      last_touch = millis();
    }
    if (touch.swiped_down() && !dispatch(TouchEvent::SWIPE_DOWN) && group_count() > 1) {
      M5.Speaker.playWav(g_snd_click_back);
      navigate_group(-1);
      last_touch = millis();
    }
    if (touch.short_pressed()) {
      if (touch.y < MENU_ZONE && page_collection.size() > 1) { open_menu(); last_touch = millis(); }
      else if (dispatch(TouchEvent::TAP)) M5.Speaker.playWav(g_snd_click_select);
    }
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
      if (_menu_open) { close_menu(); return; }
      M5.Speaker.playWav(g_snd_click_select);
      last_touch = millis();
      if (!(screensaver && screensaver->is_active())) dispatch(TouchEvent::LONG_PRESS);
    }
  }

  void navigate_next_page() {
    if (page_collection.empty()) return;
    select_page(next_index(current_page));  // within the group
  }

  void navigate_prev_page() {
    if (page_collection.empty()) return;
    select_page(prev_index(current_page));
  }
};

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_DISPLAY_MANAGER)
inline DisplayManager display_manager;  // C++17 inline: one instance, header may be included by several .cpp
#endif