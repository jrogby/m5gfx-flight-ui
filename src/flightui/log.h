#pragma once
#include <M5Unified.h>

// Lib-internal log macros (M5.Log), independent of host app logger.
// ui::log_level filters before formatting; the host app sets it (e.g. from its "ui" log category).
namespace ui {
inline esp_log_level_t log_level = ESP_LOG_INFO;
}

#define __FUI_LOG_FORMAT(letter,format) "[%6u][" #letter "][%s:%u] %s(): [ui] " format, (unsigned long)(esp_timer_get_time()/1000ULL),m5::Log_Class::pathToFileName(__FILE__),__LINE__,__FUNCTION__
#define __FUI_LOG(lvl,letter,format,...) do { if ((lvl) <= ui::log_level) M5.Log(lvl, __FUI_LOG_FORMAT(letter,format), ##__VA_ARGS__); } while (0)
#define FUI_LOGE(format,...) __FUI_LOG(ESP_LOG_ERROR, E, format, ##__VA_ARGS__)
#define FUI_LOGW(format,...) __FUI_LOG(ESP_LOG_WARN,  W, format, ##__VA_ARGS__)
#define FUI_LOGI(format,...) __FUI_LOG(ESP_LOG_INFO,  I, format, ##__VA_ARGS__)
#define FUI_LOGD(format,...) __FUI_LOG(ESP_LOG_DEBUG, D, format, ##__VA_ARGS__)
