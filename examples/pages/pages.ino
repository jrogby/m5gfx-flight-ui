// Two swipeable pages with DisplayManager. Swipe left/right to change page, tap the button on page 2.
// Also compiles every public header (CI check that the headers build on their own).
#include <M5Unified.h>
#include <ReactESP.h>
#include <FlightUIFonts.h>
#include <flightui/DisplayManager.h>
#include <flightui/Modal.h>
#include <flightui/sfx.h>
#include <flightui/drawers/AnnunciationBoard.h>
#include <flightui/drawers/PageRowDrawer.h>
#include <flightui/drawers/TableDrawers.h>
#include <flightui/drawers/TouchButton.h>

using namespace reactesp;

EventLoop loop_;
M5Canvas canvas(&M5.Display);

struct HelloPage : IScreenPage {
  void setup() override {}
  void update() override {
    canvas.fillScreen(TFT_BLACK);
    canvas.loadFont(flightui::font::ECAMFontRegular22);
    canvas.setTextColor(ui::color::AIRBUS_GREEN);
    canvas.drawString("HELLO FLIGHT-UI", 20, 100);
  }
  void kill() override {}
};

struct ButtonPage : IScreenPage {
  ui::TouchButton btn{.x = 100, .y = 90, .w = 120, .h = 50, .label = "PUSH", .toggle = true};
  void setup() override {}
  void update() override {
    canvas.fillScreen(TFT_BLACK);
    btn.draw(canvas);
  }
  bool on_touch(const TouchEvent& e) override {
    if(e.type == TouchEvent::TAP && btn.hit(e.x, e.y)) { btn.press(); return true; }
    return false;
  }
  void kill() override {}
};

HelloPage hello;
ButtonPage button;

void setup() {
  M5.begin();
  canvas.setPsram(true);
  canvas.createSprite(M5.Display.width(), M5.Display.height());
  display_manager.add_page(&hello);
  display_manager.add_page(&button);
  display_manager.begin(loop_, canvas, M5.Display);
  loop_.onRepeat(10, [] { M5.update(); });
}

void loop() { loop_.tick(); }
