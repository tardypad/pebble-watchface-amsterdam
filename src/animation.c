/**
 * Copyright (c) 2015 Damien Tardy-Panis
 *
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE', which is part of this source code package.
 **/

#include <pebble.h>

#include "config.h"

typedef struct {
  uint32_t resource_id;
  uint8_t full_frame_index;
} XXXAnimation;

static XXXAnimation s_xxx_animations[3] = {
  { RESOURCE_ID_ANIMATION_SLIDE, 4 },
  { RESOURCE_ID_ANIMATION_SCALE, 5 },
  { RESOURCE_ID_ANIMATION_ROLL, 4 }
};

static BitmapLayer *s_animation_bitmap_layer;
static GBitmap *s_animation_bitmap = NULL;
static GBitmapSequence *s_animation_sequence = NULL;
static bool s_animation_running = false;
static XXXAnimation* current_animation = NULL;

static char s_time_text[] = "     ";
static char s_next_time_text[] = "     ";
static char s_date_text[] = "      ";
static char s_next_date_text[] = "      ";

char* animation_time_text()
{
  return s_time_text;
}

char* animation_date_text()
{
  return s_date_text;
}

static void update_animation(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_bounds(layer);
  graphics_context_set_compositing_mode(ctx, GCompOpSet);
  graphics_draw_bitmap_in_rect(ctx, s_animation_bitmap, bounds);
}

static void animation_sequence_timer_handler(void *context) {
  uint32_t next_delay;

  if (gbitmap_sequence_get_current_frame_idx(s_animation_sequence) == current_animation->full_frame_index) {
    strncpy(s_time_text, s_next_time_text, sizeof(s_next_time_text));
    strncpy(s_date_text, s_next_date_text, sizeof(s_next_date_text));
  }

  if (gbitmap_sequence_update_bitmap_next_frame(s_animation_sequence, s_animation_bitmap, &next_delay)) {
    s_animation_running = true;
    bitmap_layer_set_bitmap(s_animation_bitmap_layer, s_animation_bitmap);
    layer_mark_dirty(bitmap_layer_get_layer(s_animation_bitmap_layer));
#ifdef DEBUG_SLOW_ANIMATIONS
    app_timer_register(DEBUG_SLOW_ANIMATIONS_DELAY, animation_sequence_timer_handler, NULL);
#else
    app_timer_register(next_delay, animation_sequence_timer_handler, NULL);
#endif
  } else {
    s_animation_running = false;
  }
}

void animation_start_sequence(struct tm* tick_time) {
  char* time_format = clock_is_24h_style() ? "%H:%M" : "%I:%M";
  strftime(s_next_time_text, sizeof(s_next_time_text), time_format, tick_time);
  strftime(s_next_date_text, sizeof(s_next_date_text), "%a %d", tick_time);

  if (s_animation_running)
    return;

  if (s_animation_sequence) {
    gbitmap_sequence_destroy(s_animation_sequence);
    s_animation_sequence = NULL;
  }
  if (s_animation_bitmap) {
    gbitmap_destroy(s_animation_bitmap);
    s_animation_bitmap = NULL;
  }

  // select animation
  size_t animations_count = sizeof(s_xxx_animations) / sizeof(s_xxx_animations[0]);
  int random_index = rand() % animations_count;
  current_animation = &s_xxx_animations[random_index];

  // initialize animation
  uint32_t animation_id = current_animation->resource_id;
  s_animation_sequence = gbitmap_sequence_create_with_resource(animation_id);
  s_animation_bitmap = gbitmap_create_blank(gbitmap_sequence_get_bitmap_size(s_animation_sequence), GBitmapFormat8Bit);

  app_timer_register(1, animation_sequence_timer_handler, NULL);
}

void animation_load(Layer *layer) {
  GRect layer_bounds = layer_get_bounds(layer);
  s_animation_bitmap_layer = bitmap_layer_create(layer_bounds);
  Layer* animation_layer = bitmap_layer_get_layer(s_animation_bitmap_layer);
  layer_add_child(layer, animation_layer);
  layer_set_update_proc(animation_layer, update_animation);
}

void animation_unload(Layer *layer) {
  bitmap_layer_destroy(s_animation_bitmap_layer);
  gbitmap_sequence_destroy(s_animation_sequence);
  gbitmap_destroy(s_animation_bitmap);
}
