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
} XXXAnimation;

static XXXAnimation s_xxx_animations[3] = {
  { RESOURCE_ID_ANIMATION_SLIDE },
  { RESOURCE_ID_ANIMATION_SCALE },
  { RESOURCE_ID_ANIMATION_ROLL }
};

static Layer *s_animation_layer;
static GDrawCommandSequence *s_animation_sequence = NULL;
static GDrawCommandFrame *s_animation_sequence_frame = NULL;
static bool s_animation_running = false;
static uint32_t s_animation_sequence_num_frames = 0;
static uint32_t s_animation_sequence_index = 0;
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
  gdraw_command_frame_draw(ctx, s_animation_sequence, s_animation_sequence_frame, GPoint(0, 0));
}

static void animation_sequence_timer_handler(void *context) {
  layer_mark_dirty(s_animation_layer);

  if (s_animation_sequence_index >= s_animation_sequence_num_frames) {
    s_animation_running = false;
    s_animation_sequence_frame = NULL;
    return;
  }

  s_animation_running = true;

  s_animation_sequence_frame = gdraw_command_sequence_get_frame_by_index(s_animation_sequence, s_animation_sequence_index);
  uint32_t frame_duration = gdraw_command_frame_get_duration(s_animation_sequence_frame);

  if (s_animation_sequence_index == s_animation_sequence_num_frames / 2) {
    strncpy(s_time_text, s_next_time_text, sizeof(s_next_time_text));
    strncpy(s_date_text, s_next_date_text, sizeof(s_next_date_text));
    frame_duration = 1000;
  }

  s_animation_sequence_index++;

#ifdef DEBUG_SLOW_ANIMATIONS
    app_timer_register(DEBUG_SLOW_ANIMATIONS_DELAY, animation_sequence_timer_handler, NULL);
#else
    app_timer_register(frame_duration, animation_sequence_timer_handler, NULL);
#endif
}

void animation_start_sequence(struct tm* tick_time) {
  char* time_format = clock_is_24h_style() ? "%H:%M" : "%I:%M";
  strftime(s_next_time_text, sizeof(s_next_time_text), time_format, tick_time);
  strftime(s_next_date_text, sizeof(s_next_date_text), "%a %d", tick_time);

  if (s_animation_running)
    return;

  if (s_animation_sequence) {
    gdraw_command_sequence_destroy(s_animation_sequence);
    s_animation_sequence = NULL;
  }

  // select animation
  size_t animations_count = sizeof(s_xxx_animations) / sizeof(s_xxx_animations[0]);
  int random_index = rand() % animations_count;
  current_animation = &s_xxx_animations[random_index];

  // initialize animation
  uint32_t animation_id = current_animation->resource_id;
  s_animation_sequence = gdraw_command_sequence_create_with_resource(animation_id);
  s_animation_sequence_num_frames = gdraw_command_sequence_get_num_frames(s_animation_sequence);
  s_animation_sequence_frame = NULL;
  s_animation_sequence_index = 0;

  app_timer_register(1, animation_sequence_timer_handler, NULL);
}

void animation_load(Layer *layer) {
  GRect layer_bounds = layer_get_bounds(layer);
  s_animation_layer = layer_create(layer_bounds);
  layer_add_child(layer, s_animation_layer);
  layer_set_update_proc(s_animation_layer, update_animation);
}

void animation_unload(Layer *layer) {
  layer_destroy(s_animation_layer);
  gdraw_command_sequence_destroy(s_animation_sequence);
}
