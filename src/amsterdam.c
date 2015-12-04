#include <pebble.h>

#define TIME_FONT_KEY FONT_KEY_BITHAM_42_MEDIUM_NUMBERS
#define TIME_FONT_HEIGHT 32
#define TIME_FONT_PADDING 10

static Window *s_main_window;
static BitmapLayer *s_stripe_bitmap_layer;
static BitmapLayer *s_animation_bitmap_layer;
static Layer *s_time_layer;
static GBitmap *s_animation_bitmap = NULL;
static GBitmapSequence *s_animation_sequence = NULL;
static char s_time_text[] = "00:00";

static void load_animation_sequence();

static void animation_sequence_timer_handler(void *context) {
  uint32_t next_delay;

  if(gbitmap_sequence_update_bitmap_next_frame(s_animation_sequence, s_animation_bitmap, &next_delay)) {
    bitmap_layer_set_bitmap(s_animation_bitmap_layer, s_animation_bitmap);
    layer_mark_dirty(bitmap_layer_get_layer(s_animation_bitmap_layer));
    app_timer_register(next_delay, animation_sequence_timer_handler, NULL);
  }
}

static void load_animation_sequence() {
  if(s_animation_sequence) {
    gbitmap_sequence_destroy(s_animation_sequence);
    s_animation_sequence = NULL;
  }
  if(s_animation_bitmap) {
    gbitmap_destroy(s_animation_bitmap);
    s_animation_bitmap = NULL;
  }

  s_animation_sequence = gbitmap_sequence_create_with_resource(RESOURCE_ID_ANIMATION_SLIDE);
  s_animation_bitmap = gbitmap_create_blank(gbitmap_sequence_get_bitmap_size(s_animation_sequence), GBitmapFormat8Bit);

  app_timer_register(1, animation_sequence_timer_handler, NULL);
}

static void update_animation(Layer *layer, GContext *ctx) {
  GRect frame = layer_get_frame(layer);
  graphics_context_set_compositing_mode(ctx, GCompOpSet);
  graphics_draw_bitmap_in_rect(ctx, s_animation_bitmap, frame);
}

static void update_time(Layer *layer, GContext *ctx) {
  GRect frame = layer_get_frame(layer);
  frame.origin.y = -TIME_FONT_PADDING;
  GFont font = fonts_get_system_font(TIME_FONT_KEY);
  graphics_context_set_text_color(ctx, GColorWhite);
  graphics_draw_text(ctx, s_time_text, font, frame, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
}

static void handle_tick(struct tm* tick_time, TimeUnits units_changed) {
  strftime(s_time_text, sizeof(s_time_text), "%H:%M", tick_time);
  load_animation_sequence();
}

static void main_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect window_bounds = layer_get_bounds(window_layer);

  int16_t stripe_height = window_bounds.size.h / 3;
  GRect stripe_bounds = GRect(
    window_bounds.origin.x,
    window_bounds.origin.y + stripe_height,
    window_bounds.size.w,
    stripe_height
  );
  s_stripe_bitmap_layer = bitmap_layer_create(stripe_bounds);
  bitmap_layer_set_background_color(s_stripe_bitmap_layer, GColorBlack);
  layer_add_child(window_layer, bitmap_layer_get_layer(s_stripe_bitmap_layer));

  int16_t time_height = TIME_FONT_HEIGHT;
  GRect time_bounds = GRect(
    0,
    (stripe_height - time_height) / 2,
    stripe_bounds.size.w,
    time_height
  );
  s_time_layer = layer_create(time_bounds);
  layer_add_child(bitmap_layer_get_layer(s_stripe_bitmap_layer), s_time_layer);
  layer_set_update_proc(s_time_layer, update_time);

  GRect animation_bounds = GRect(
    0,
    0,
    stripe_bounds.size.w,
    stripe_bounds.size.h
  );
  s_animation_bitmap_layer = bitmap_layer_create(animation_bounds);
  layer_add_child(bitmap_layer_get_layer(s_stripe_bitmap_layer), bitmap_layer_get_layer(s_animation_bitmap_layer));
  layer_set_update_proc(bitmap_layer_get_layer(s_animation_bitmap_layer), update_animation);

  time_t now = time(NULL);
  struct tm *current_time = localtime(&now);
  handle_tick(current_time, MINUTE_UNIT);
  tick_timer_service_subscribe(MINUTE_UNIT, handle_tick);
}

static void main_window_unload(Window *window) {
  tick_timer_service_unsubscribe();
  bitmap_layer_destroy(s_stripe_bitmap_layer);
  bitmap_layer_destroy(s_animation_bitmap_layer);
  gbitmap_sequence_destroy(s_animation_sequence);
  gbitmap_destroy(s_animation_bitmap);
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
