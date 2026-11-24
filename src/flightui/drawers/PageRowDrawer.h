#pragma once
#include "flightui/types.h"
#include "flightui/color.h"
#include "flightui/fonts/vlw.h"
#include <vector>
#include <unordered_set>
#include <unordered_map>


/**
 * @brief Class to draw a row of data on the M5Canvas.
 */
class PageRowDrawer {
 public:
  PageRowDrawer(M5Canvas* page_canvas) : page(page_canvas), currentRow(0) {}

  // Resets the row counter
  void reset() {
    currentRow = 0;
  }

  // Draws a static row with label and unit (only once)
  void drawStaticRow(const String& label, const String& unit, const uint16_t labelColor = ui::color::ECAM_LABEL, const uint16_t unitColor = ui::color::ECAM_UNIT) {
    if (drawnRows.count(currentRow)) { // Skip if already drawn
      currentRow++;
      return;
    }

    page->setTextColor(labelColor); page->setTextDatum(textdatum_t::top_left); page->loadFont(ECAMFontRegular18);
    page->drawString(label, 0, currentRow * rowHeight);      // Label

    // Store label for later use in updateRowValueWithSource
    rowLabels[currentRow] = label;
    labelWidths[currentRow] = page->textWidth(label);

    page->setTextColor(unitColor); page->setTextDatum(textdatum_t::top_left);  page->loadFont(ECAMFontRegular14);
    page->drawString(unit, 282, currentRow * rowHeight + 3); // Unit

    drawnRows.insert(currentRow);  // Mark row as drawn
    currentRow++;
    page->loadFont(ECAMFontRegular18); // Ensure value font is set for next updates
  }

  // Updates only the value in a specific row
  void updateRowValue(const String& value, const uint16_t valueColor = ui::color::ECAM_VALUE) {
    int y = currentRow * rowHeight;

    // Clear the entire value area (fixed width to avoid text remnants) - Value area is from after label to before unit (x: 180 to 280)
    page->fillRect(180, y, 100, 18, ui::color::ECAM_BG);
    page->loadFont(ECAMFontRegular18); page->setTextColor(valueColor); page->setTextDatum(textdatum_t::top_right); page->drawString(value, 280, y); // Value (right-aligned)

    currentRow++;
  }

  // Updates value with source shown in label area (compact layout: "TWS  OWM    1.2 MPS")
  void updateRowValueWithSource(const String& value, const char* source) {  // source: short label, e.g. "N2K"
    updateRowValueWithSource(value, source, ui::color::ECAM_VALUE, ui::color::AIRBUS_PURPLE);
  }

  void updateRowValueWithSource(const String& value, const char* source, uint16_t valueColor, uint16_t sourceColor) {
    int y = currentRow * rowHeight;
    int row = currentRow;

    // Get stored label width (or calculate if not found)
    int labelWidth = labelWidths.count(row) ? labelWidths[row] : 60; // Default 60px if not found
    int clearStartX = labelWidth + 5;  // Start clearing 5px after label ends
    int clearWidth = 280 - clearStartX; // Clear from after label to value end

    // Clear source area (after label) and value area
    page->fillRect(clearStartX, y, clearWidth, 18, ui::color::ECAM_BG);

    // Draw source (always, since area cleared above)
    page->loadFont(ECAMFontRegular10);
    page->setTextColor(sourceColor);
    page->setTextDatum(textdatum_t::top_left);
    page->drawString(source, labelWidth + 10, y + 3); // Draw source after label (small, dim)

    page->loadFont(ECAMFontRegular18);
    page->setTextColor(valueColor);
    page->setTextDatum(textdatum_t::top_right);
    page->drawString(value, 280, y); // Draw value (right-aligned)

    currentRow++;
  }

  // Sets the row index manually (useful for skipping rows)
  void setRowIndex(int row) { currentRow = row; }

  // Gets the current row index
  int getRowIndex() const { return currentRow; }

 private:
  M5Canvas* page;
  int currentRow;  // Tracks the current row number
  const int rowHeight = 21;  // Height of each row
  std::unordered_set<int> drawnRows;  // Tracks rows that have already been drawn
  std::unordered_map<int, String> rowLabels;  // Stores label text for each row
  std::unordered_map<int, int> labelWidths;  // Stores label pixel width for each row
};



/**
 * @brief Class to draw a row of data on the M5Canvas.
 */
class OldPageRowDrawer {
 public:
  OldPageRowDrawer(M5Canvas* page_canvas) : page(page_canvas) {}

  // Draws a row of data with label, value, and unit
  void drawFullRow(const String& label, const String& value, const String& unit, const int y,
      const uint16_t color = ui::color::ECAM_LABEL,
      const uint16_t valueColor = ui::color::ECAM_VALUE,
      const uint16_t unitColor = ui::color::ECAM_UNIT) {
    drawStaticLabelRow(label, y, color); // Label
    page->loadFont(ECAMFontRegular18); page->setTextColor(valueColor); page->setTextDatum(textdatum_t::top_right); page->drawString(value, 280, y   );  // Value
    page->loadFont(ECAMFontRegular14); page->setTextColor(unitColor);  page->setTextDatum(textdatum_t::top_left);  page->drawString(unit , 282, y +3);  // Unit
  }

  // Draws a label row with the specified label and y position
  void drawStaticLabelRow(const String& label, int y, const uint16_t labelColor = ui::color::ECAM_LABEL) {
    page->loadFont(ECAMFontRegular18); page->setTextColor(labelColor); page->setTextDatum(textdatum_t::top_left); page->drawString(label, 0, y); // Label
  }


  // Draws static elements (label + unit)
  void drawStaticRow(const String& label, const String& unit, const int y,
      const uint16_t labelColor = ui::color::ECAM_LABEL,
      const uint16_t unitColor = ui::color::ECAM_UNIT) {
    page->loadFont(ECAMFontRegular18); page->setTextColor(labelColor); page->setTextDatum(textdatum_t::top_left); page->drawString(label, 0, y);     // Label
    page->loadFont(ECAMFontRegular14); page->setTextColor(unitColor);  page->setTextDatum(textdatum_t::top_left); page->drawString(unit , 282, y +3); // Unit
  }

  // Updates only the value dynamically
  void updateRowValue(const String& value, const int y, const uint16_t valueColor = ui::color::ECAM_VALUE) {
    // Clear only the area where the value is drawn
    page->fillRect(200, y, 80, 18, ui::color::ECAM_BG);  // Adjust height (18) to match the font size
    page->loadFont(ECAMFontRegular18); page->setTextColor(valueColor); page->setTextDatum(textdatum_t::top_right); page->drawString(value, 280, y); // Value (right-aligned)
  }

 private:
  M5Canvas* page;

};
