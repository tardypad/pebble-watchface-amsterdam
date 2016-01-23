/**
 * Copyright (c) 2015 Damien Tardy-Panis
 *
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE', which is part of this source code package.
 **/

#pragma once

//#define DEBUG_SLOW_ANIMATIONS
//#define DEBUG_SLOW_ANIMATIONS_DELAY 1000

#define TIME_FONT_KEY FONT_KEY_BITHAM_42_MEDIUM_NUMBERS
#define TIME_FONT_HEIGHT 32
#define TIME_FONT_PADDING 10

#define DATE_FONT_KEY FONT_KEY_GOTHIC_18_BOLD
#define DATE_LAYER_HEIGHT 20
#define DATE_BOTTOM_MARGIN PBL_IF_ROUND_ELSE(10, 5)

typedef struct {
  uint32_t resource_id;
  uint8_t full_frame_index;
} XXXAnimation;

XXXAnimation xxx_animations[3] = {
  { RESOURCE_ID_ANIMATION_SLIDE, 4 },
  { RESOURCE_ID_ANIMATION_SCALE, 5 },
  { RESOURCE_ID_ANIMATION_ROLL, 4 }
};