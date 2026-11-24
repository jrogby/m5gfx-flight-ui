#pragma once
#include "flightui/types.h"
#include "flightui/color.h"
#include "flightui/fonts/vlw/ECAMFontRegular.h"
#include <vector>
#include <unordered_set>




class DynamicTableOptimizedDrawer {
 public:
  DynamicTableOptimizedDrawer(M5Canvas* page_canvas) : page(page_canvas), currentY(0), drawRowLines(false), drawColumnLines(false) {}

  // Sets the column definitions for the table
  void setColumns(const std::vector<String>& column_headers, const std::vector<int>& column_widths,
                  const uint16_t color = ui::color::ECAM_LABEL, const uint16_t color_bg = TFT_TRANSPARENT) {
    col_widths = column_widths;
    header_color_fg = color;
    header_color_bg = color_bg;

    // Convert all column headers to uppercase once
    col_headers.clear();
    col_headers.reserve(column_headers.size());
    for (const auto& header : column_headers) {
      String upper = header;
      upper.toUpperCase();
      col_headers.push_back(upper);
    }
  }

  // Draws the table header (optimized to avoid redundant operations)
  void drawHeader() {
    int x = 0;
    page->setTextDatum(textdatum_t::top_left);
    page->loadFont(ECAMFontRegular14);
    page->setTextColor(header_color_fg, header_color_bg);

    // Draw the background for the header row only if needed
    if (header_color_bg != TFT_TRANSPARENT) {
      page->fillRect(0, currentY, page->width(), row_height, header_color_bg);
    }

    // Draw each column header
    for (size_t i = 0; i < col_headers.size(); ++i) {
      page->drawString(col_headers[i], x, currentY);
      x += col_widths[i];
    }

    // Draw horizontal line below the header if row lines are enabled
    if (drawRowLines) {
      page->drawLine(0, currentY + row_height - 2, page->width(), currentY + row_height - 2, lineColor);
    }

    currentY += row_height;  // Move to the next row
  }

  // Draws a row of data (optimized to minimize redundant operations)
  void drawRow(const std::vector<String>& rowData, const uint16_t color = ui::color::ECAM_VALUE, const uint16_t color_bg = TFT_TRANSPARENT) {
    int x = 0;
    page->setTextDatum(textdatum_t::top_left);
    page->setTextColor(color, color_bg);
    page->loadFont(ECAMFontRegular14);

    // Draw the background for the row only if needed
    if (color_bg != TFT_TRANSPARENT) {
      page->fillRect(0, currentY, page->width(), row_height, color_bg);
    }

    // Draw each cell in the row
    for (size_t i = 0; i < rowData.size(); ++i) {
      page->drawString(rowData[i], x, currentY);

      // Draw vertical lines only if column lines are enabled
      if (drawColumnLines && i < rowData.size() - 1) {
        page->drawLine(x + col_widths[i] - 1, currentY, x + col_widths[i] - 1, currentY + row_height, lineColor);
      }
      x += col_widths[i];
    }

    // Draw horizontal line below the row if row lines are enabled
    if (drawRowLines) {
      page->drawLine(0, currentY + row_height - 2, page->width(), currentY + row_height - 2, lineColor);
    }

    currentY += row_height;  // Move to the next row
  }

  // Enables or disables grid lines
  void enableGridLines(bool rowLines, bool columnLines, uint16_t color = TFT_WHITE) {
    drawRowLines = rowLines;
    drawColumnLines = columnLines;
    lineColor = color;
  }

  // Resets the table drawer (optimized to avoid unnecessary clearing)
  void reset(bool clearCanvas = true) {
    if (clearCanvas) {
      page->clear();
    }
    currentY = 0;
  }

 private:
  M5Canvas* page;
  int currentY;
  std::vector<String> col_headers;
  std::vector<int> col_widths;
  uint16_t header_color_fg;
  uint16_t header_color_bg;

  uint16_t lineColor;
  bool drawRowLines;
  bool drawColumnLines;
  const int row_height = 22;
};






/**
 * @brief Class to draw a table on the M5Canvas.
 */
class DynamicTableDrawer {
 public:
  DynamicTableDrawer(M5Canvas* page_canvas) : page(page_canvas), currentY(0) {}

  // Sets the column definitions for the table
  void setColumns(const std::vector<String>& column_headers, const std::vector<int>& column_widths,
      const uint16_t color = ui::color::ECAM_LABEL,
      const uint16_t color_bg = TFT_BLACK) {

    this->col_widths = column_widths;
    this->header_color_fg = color;
    this->header_color_bg = color_bg;

    // Convert all column headers to uppercase
    this->col_headers.clear();
    for (const auto& header : column_headers) {
      String upper = header;
      upper.toUpperCase();
      this->col_headers.push_back(upper);
    }
  }

  // Draws the table header
  void drawHeader() {
    int x = 0;
    page->setTextDatum(textdatum_t::top_left);
    // size 12 was originall...
    page->loadFont(ECAMFontRegular14);
    page->setTextColor(header_color_fg, header_color_bg);

    // Draw the background for the header row
    if (header_color_bg != TFT_TRANSPARENT) { page->fillRect(0, currentY, page->width(), row_height, header_color_bg); }

    // Draw each column header
    for (size_t i = 0; i < col_headers.size(); ++i) { page->drawString(col_headers[i], x, currentY); x += col_widths[i]; }

    // Draw horizontal line below the header if row lines are enabled
    if (drawRowLines) { page->drawLine(0, currentY + row_height, page->width(), currentY + row_height, lineColor); }

    currentY += row_height;  // Move to the next row
  }

  // Draws a row of data
  void drawRow(const std::vector<String>& rowData, const uint16_t color = ui::color::ECAM_VALUE, const uint16_t color_bg = TFT_BLACK) {
    int x = 0;
    page->setTextDatum(textdatum_t::top_left);
    page->setTextColor(color, color_bg);
    page->loadFont(ECAMFontRegular14);

    // Draw the background for the row
    if (color_bg != TFT_TRANSPARENT) { page->fillRect(0, currentY, page->width(), row_height, color_bg); }

    // Draw each cell in the row & vertical lines if enabled
    for (size_t i = 0; i < rowData.size(); ++i) {
      page->drawString(rowData[i], x, currentY);
      if (drawColumnLines && i < rowData.size() - 1) { page->drawLine(x + col_widths[i]-5, currentY, x + col_widths[i]-5, currentY + row_height, lineColor);}
      x += col_widths[i];
    }

    // Draw horizontal line below the row if row lines are enabled
    if (drawRowLines) { page->drawLine(1, currentY + row_height-2, page->width() -1, currentY + row_height-2, lineColor); }

    currentY += row_height;  // Move to the next row
  }

  // Enables or disables grid lines
  void enableGridLines(bool rowLines, bool columnLines, uint16_t color = TFT_WHITE) {
    drawRowLines = rowLines;
    drawColumnLines = columnLines;
    lineColor = color;
  }

  // Resets the table drawer
  void reset() {
    page->clear();
    page->setCursor(0, 0);
    currentY = 0;
  }

 private:
  M5Canvas* page;
  int currentY;
  std::vector<String> col_headers;
  std::vector<int> col_widths;
  uint16_t header_color_fg;
  uint16_t header_color_bg;

  uint16_t lineColor;
  bool drawRowLines;
  bool drawColumnLines;
  const int row_height = 22;
};


/** Class to draw a table with the configured NMEA2000 iDevs. */
class N2kTableDrawer {
 public:
  N2kTableDrawer(M5Canvas* page_canvas) : page(page_canvas), currentY(0) {}

  // Draws a header with the specified text and color
  void drawHeader(const String& header, const uint16_t color) {
    page->setTextDatum(textdatum_t::top_left);
    page->loadFont(ECAMFontRegular12);
    page->setTextColor(color);
    page->drawString(header, 10, currentY);
    currentY += 20;
  }

  // Draws a row of data with device, source, class/function, and model ID
  void drawRow(int dev, int src, int devClass, int devFunction, const String& modelID, const uint16_t color) {
    page->setTextDatum(textdatum_t::top_left);
    page->setTextColor(color);
    page->loadFont(ECAMFontRegular12);
    page->drawString(String(dev), 10, currentY);
    page->drawString(String(src), 50, currentY);
    page->drawString(String(devClass) + "/" + String(devFunction), 90, currentY);
    page->drawString(modelID, 150, currentY);
    currentY += 20;
  }

  void reset() {
    page->clear();
    page->setCursor(0, 0);
    currentY = 0;
  }

 private:
  M5Canvas* page;
  int currentY;
};