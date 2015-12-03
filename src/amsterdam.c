#include <pebble.h>

#define TIME_FONT_KEY FONT_KEY_BITHAM_42_MEDIUM_NUMBERS
#define TIME_FONT_HEIGHT 32
#define TIME_FONT_PADDING 10

static Window *s_main_window;
static BitmapLayer *s_bitmap_layer;
static Layer *s_time_layer;
static char s_time_text[] = "00:00";

static void update_time(Layer *layer, GContext *ctx) {
  GRect frame = layer_get_frame(layer);
  frame.origin.y = -TIME_FONT_PADDING;
  GFont font = fonts_get_system_font(TIME_FONT_KEY);
  graphics_context_set_text_color(ctx, GColorWhite);
  graphics_draw_text(ctx, s_time_text, font, frame, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
}

static void handle_tick(struct tm* tick_time, TimeUnits units_changed) {
  strftime(s_time_text, sizeof(s_time_text), "%H:%M", tick_time);
  layer_mark_dirty(s_time_layer);
}

static void main_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect window_bounds = layer_get_bounds(window_layer);

  int16_t stripe_height = window_bounds.size.h / 3;
  GRect stripe_bounds = (GRect) {
    .origin = (GPoint) { .x = window_bounds.origin.x, .y = window_bounds.origin.y + stripe_height },
    .size   = (GSize) { .w = window_bounds.size.w, .h = stripe_height },
  };
  s_bitmap_layer = bitmap_layer_create(stripe_bounds);
  bitmap_layer_set_background_color(s_bitmap_layer, GColorBlack);
  layer_add_child(window_layer, bitmap_layer_get_layer(s_bitmap_layer));

  int16_t time_height = TIME_FONT_HEIGHT;
  GRect time_bounds = (GRect) {
    .origin = (GPoint) { .x = 0, .y = (stripe_height - time_height) / 2},
    .size   = (GSize) { .w = stripe_bounds.size.w, .h = time_height },
  };
  s_time_layer = layer_create(time_bounds);
  layer_add_child(bitmap_layer_get_layer(s_bitmap_layer), s_time_layer);
  layer_set_update_proc(s_time_layer, update_time);

  time_t now = time(NULL);
  struct tm *current_time = localtime(&now);
  handle_tick(current_time, MINUTE_UNIT);
  tick_timer_service_subscribe(MINUTE_UNIT, handle_tick);
}

static void main_window_unload(Window *window) {
  tick_timer_service_unsubscribe();
  bitmap_layer_destroy(s_bitmap_layer);
  layer_destroy(s_time_layer);
}

static void init() {
  s_main_window = window_create();
  window_set_background_color(s_main_window, GColorRed);
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload,
  });
  window_stack_push(s_main_window, true);
}

static void deinit() {
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
