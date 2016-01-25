/**
 * Copyright (c) 2015 Damien Tardy-Panis
 *
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE', which is part of this source code package.
 **/

#include <pebble.h>

#include "animation.h"
#include "config.h"

static BitmapLayer *s_stripe_bitmap_layer;
static Layer *s_time_layer;
static TextLayer *s_date_text_layer;

static void update_time(Layer *layer, GContext *ctx) {
  char* time_text = animation_time_text();

  if (strcmp(time_text, "     ") == 0)
    return;

  GRect bounds = layer_get_bounds(layer);
  bounds.origin.y = -TIME_FONT_PADDING;
  GFont font = fonts_get_system_font(TIME_FONT_KEY);
  graphics_context_set_text_color(ctx, GColorWhite);
  graphics_draw_text(ctx, time_text, font, bounds, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
}

static void handle_tick(struct tm* tick_time, TimeUnits units_changed) {
  animation_start_sequence(tick_time);
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
  text_layer_set_text(s_date_text_layer, animation_date_text());
  text_layer_set_text_alignment(s_date_text_layer, GTextAlignmentCenter);
  text_layer_set_font(s_date_text_layer, fonts_get_system_font(DATE_FONT_KEY));
  text_layer_set_text_color(s_date_text_layer, GColorBlack);
  text_layer_set_background_color(s_date_text_layer, GColorClear);
  layer_add_child(window_layer, text_layer_get_layer(s_date_text_layer));

  animation_load(stripe_layer);

  time_t now = time(NULL);
  struct tm *current_time = localtime(&now);
  handle_tick(current_time, MINUTE_UNIT);
  tick_timer_service_subscribe(MINUTE_UNIT, handle_tick);
}

void main_window_unload(Window *window) {
  tick_timer_service_unsubscribe();
  bitmap_layer_destroy(s_stripe_bitmap_layer);
  layer_destroy(s_time_layer);
  text_layer_destroy(s_date_text_layer);

  animation_unload(bitmap_layer_get_layer(s_stripe_bitmap_layer));
}