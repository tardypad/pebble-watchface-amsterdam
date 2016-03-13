/**
 * Copyright (c) 2015 Damien Tardy-Panis
 *
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE', which is part of this source code package.
 **/

#pragma once

#define LOG_DEBUG(...) app_log(APP_LOG_LEVEL_DEBUG, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_INFO(...) app_log(APP_LOG_LEVEL_INFO, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_WARN(...) app_log(APP_LOG_LEVEL_WARNING, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_ERROR(...) app_log(APP_LOG_LEVEL_ERROR, __FILE__, __LINE__, __VA_ARGS__)

#ifndef DEBUG_LOGS
  #undef LOG_DEBUG
  #define LOG_DEBUG(...)
#endif

//#define DEBUG_SLOW_ANIMATIONS_DELAY 1000

#define TIME_FONT_KEY FONT_KEY_BITHAM_42_MEDIUM_NUMBERS
#define TIME_FONT_HEIGHT 32
#define TIME_FONT_PADDING 10

#define DATE_FONT_KEY FONT_KEY_GOTHIC_18_BOLD
#define DATE_LAYER_HEIGHT 20
#define DATE_BOTTOM_MARGIN PBL_IF_ROUND_ELSE(10, 5)
