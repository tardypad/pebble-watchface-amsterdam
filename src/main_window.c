/**
 * Copyright (c) 2015 Damien Tardy-Panis
 *
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE', which is part of this source code package.
 **/

#include <pebble.h>

#include "config.h"

static BitmapLayer *s_stripe_bitmap_layer;
static BitmapLayer *s_animation_bitmap_layer;
static Layer *s_time_layer;
static TextLayer *s_date_text_layer;
static GBitmap *s_animation_bitmap = NULL;
static GBitmapSequence *s_animation_sequence = NULL;
static char s_time_text[] = "     ";
static char s_next_time_text[] = "     ";
static char s_date_text[] = "      ";
static bool s_animation_running = false;
static XXXAnimation* current_animation = NULL;

static void load_animation_sequence();

static void animation_sequence_timer_handler(void *context) {
  uint32_t next_delay;

  if (gbitmap_sequence_get_current_frame_idx(s_animation_sequence) == current_animation->full_frame_index) {
    strncpy(s_time_text, s_next_time_text, sizeof(s_next_time_text));
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

static void select_animation()
{
  size_t animations_count = sizeof(xxx_animations) / sizeof(xxx_animations[0]);
  int random_index = rand() % animations_count;
  current_animation = &xxx_animations[random_index];
}

static void load_animation_sequence() {
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

  select_animation();

  uint32_t animation_id = current_animation->resource_id;
  s_animation_sequence = gbitmap_sequence_create_with_resource(animation_id);
  s_animation_bitmap = gbitmap_create_blank(gbitmap_sequence_get_bitmap_size(s_animation_sequence), GBitmapFormat8Bit);

  app_timer_register(1, animation_sequence_timer_handler, NULL);
}

static void update_animation(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_bounds(layer);
  graphics_context_set_compositing_mode(ctx, GCompOpSet);
  graphics_draw_bitmap_in_rect(ctx, s_animation_bitmap, bounds);
}

static void update_time(Layer *layer, GContext *ctx) {
  if (strcmp(s_time_text, "     ") == 0)
    return;

  GRect bounds = layer_get_bounds(layer);
  bounds.origin.y = -TIME_FONT_PADDING;
  GFont font = fonts_get_system_font(TIME_FONT_KEY);
  graphics_context_set_text_color(ctx, GColorWhite);
  graphics_draw_text(ctx, s_time_text, font, bounds, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
}

static void handle_tick(struct tm* tick_time, TimeUnits units_changed) {
  char* time_format = clock_is_24h_style() ? "%H:%M" : "%I:%M";
  strftime(s_next_time_text, sizeof(s_next_time_text), time_format, tick_time);

  load_animation_sequence();

   if ((tick_time->tm_hour == 0 && tick_time->tm_min <= 1) || strcmp(s_date_text, "      " ) == 0) {
     strftime(s_date_text, sizeof(s_date_text), "%a %d", tick_time);
   }
}

void main_window_load(Window *window) {
  window_set_background_color(window, GColorRed);

  Layer *window_layer = window_get_root_layer(window);
  GRect window_bounds = layer_get_bounds(window_layer);

  int16_t stripe_height = window_bounds.size.h / 3;
  GRect stripe_frame = GRect(
    window_bounds.origin.x,
    window_bounds.origin.y + stripe_height,
    window_bounds.size.w,
    stripe_height
  );
  s_stripe_bitmap_layer = bitmap_layer_create(stripe_frame);
  Layer* stripe_layer = bitmap_layer_get_layer(s_stripe_bitmap_layer);
  bitmap_layer_set_background_color(s_stripe_bitmap_layer, GColorBlack);
  layer_add_child(window_layer, stripe_layer);

  int16_t time_height = TIME_FONT_HEIGHT;
  GRect time_frame = GRect(
    0,
    (stripe_height - time_height) / 2,
    stripe_frame.size.w,
    time_height
  );
  s_time_layer = layer_create(time_frame);
  layer_add_child(stripe_layer, s_time_layer);
  layer_set_update_proc(s_time_layer, update_time);

  int16_t date_height = DATE_LAYER_HEIGHT;
  GRect date_frame = GRect(
    window_bounds.origin.x,
    window_bounds.origin.y + window_bounds.size.h - (DATE_BOTTOM_MARGIN + date_height),
    window_bounds.size.w,
    date_height
  );
  s_date_text_layer = text_layer_create(date_frame);
  text_layer_set_text(s_date_text_layer, s_date_text);
  text_layer_set_text_alignment(s_date_text_layer, GTextAlignmentCenter);
  text_layer_set_font(s_date_text_layer, fonts_get_system_font(DATE_FONT_KEY));
  text_layer_set_text_color(s_date_text_layer, GColorBlack);
  text_layer_set_background_color(s_date_text_layer, GColorClear);
  layer_add_child(window_layer, text_layer_get_layer(s_date_text_layer));

  GRect animation_frame = GRect(
    0,
    0,
    stripe_frame.size.w,
    stripe_frame.size.h
  );
  s_animation_bitmap_layer = bitmap_layer_create(animation_frame);
  Layer* animation_layer = bitmap_layer_get_layer(s_animation_bitmap_layer);
  layer_add_child(stripe_layer, animation_layer);
  layer_set_update_proc(animation_layer, update_animation);

  time_t now = time(NULL);
  struct tm *current_time = localtime(&now);
  handle_tick(current_time, MINUTE_UNIT);
  tick_timer_service_subscribe(MINUTE_UNIT, handle_tick);
}

void main_window_unload(Window *window) {
  tick_timer_service_unsubscribe();
  bitmap_layer_destroy(s_stripe_bitmap_layer);
  bitmap_layer_destroy(s_animation_bitmap_layer);
  gbitmap_sequence_destroy(s_animation_sequence);
  gbitmap_destroy(s_animation_bitmap);
  layer_destroy(s_time_layer);
  text_layer_destroy(s_date_text_layer);
}