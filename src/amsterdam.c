#include <pebble.h>

static Window *s_main_window;
static BitmapLayer *s_bitmap_layer;

static void main_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect window_bounds = layer_get_bounds(window_layer); 
  int16_t stripe_height = window_bounds.size.h / 3;
  GRect bounds = (GRect) {
    .origin = (GPoint) { .x = window_bounds.origin.x, .y = window_bounds.origin.y + stripe_height },
    .size   = (GSize) { .w = window_bounds.size.w, .h = stripe_height },
  };
  s_bitmap_layer = bitmap_layer_create(bounds);
  bitmap_layer_set_background_color(s_bitmap_layer, GColorBlack);
  layer_add_child(window_layer, bitmap_layer_get_layer(s_bitmap_layer));
}

static void main_window_unload(Window *window) {
  bitmap_layer_destroy(s_bitmap_layer);
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
