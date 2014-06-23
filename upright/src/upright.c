#include <pebble.h>

static Window *window;
static Layer *circle_layer;
static Layer *time_layer;

static GBitmap *digits[10] = {
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL
};

static int16_t current_hour = 0;
static int16_t current_minute = 0;

#define DRAW_WITH_IMAGES 1
#if !DRAW_WITH_IMAGES
#include "upright_font.h"
#endif

// drawing

static void circle_layer_draw(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_bounds(layer);
  int16_t radius = bounds.size.w / 2 - 2;
  GPoint center = {
    bounds.origin.x + bounds.size.w / 2,
    bounds.origin.y + bounds.size.h / 2
  };
  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_fill_rect(ctx, bounds, 0, GCornerNone);

  graphics_context_set_fill_color(ctx, GColorWhite);
  graphics_fill_circle(ctx, center, radius);
  // graphics_context_set_stroke_color(ctx, GColorWhite);
  // graphics_draw_circle(ctx, dialCenter, radius);
}

static void time_layer_draw(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_bounds(layer);
  graphics_context_set_fill_color(ctx, GColorWhite);
  graphics_fill_rect(ctx, bounds, 0, GCornerNone);
  graphics_context_set_fill_color(ctx, GColorBlack);

#if DRAW_WITH_IMAGES
  int8_t digit_1 = current_hour / 10;
  int8_t digit_2 = current_hour % 10;
  int8_t digit_3 = current_minute / 10;
  int8_t digit_4 = current_minute % 10;

  GRect digit_bounds = digits[0]->bounds;
  graphics_draw_bitmap_in_rect(ctx, digits[digit_1], digit_bounds);

  digit_bounds.origin.x += 24;
  graphics_draw_bitmap_in_rect(ctx, digits[digit_2], digit_bounds);

  digit_bounds.origin.x += 24;
  graphics_draw_bitmap_in_rect(ctx, digits[digit_3], digit_bounds);

  digit_bounds.origin.x += 24;
  graphics_draw_bitmap_in_rect(ctx, digits[digit_4], digit_bounds);
#else
  GPath *digit_path_ref = NULL;

  // 1st
  digit_path_ref = gpath_create(&DIGIT_GLYPH_3);
  gpath_draw_filled(ctx, digit_path_ref);
  gpath_destroy(digit_path_ref);

  // 2nd
  digit_path_ref = gpath_create(&DIGIT_GLYPH_1);
  gpath_move_to(digit_path_ref, GPoint(24, 0));
  gpath_draw_filled(ctx, digit_path_ref);
  gpath_destroy(digit_path_ref);

#endif


}

// time keeping

static void handle_minute_tick(struct tm *tick_time, TimeUnits units_changed) {
  current_hour = tick_time->tm_hour;
  current_minute = tick_time->tm_min;
  layer_mark_dirty(circle_layer);
  if (units_changed & HOUR_UNIT) {
    vibes_double_pulse();
  }
}

// window

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  window_set_background_color(window, GColorBlack);

  GRect bounds = layer_get_bounds(window_layer);

  int16_t circle_margin = 10;
  GSize circle_size = {
    bounds.size.w - circle_margin * 2,
    bounds.size.h - circle_margin * 2
  };
  GPoint circle_origin = {
    (bounds.size.w - circle_size.w) / 2,
    (bounds.size.h - circle_size.h) / 2
  };
  GRect circle_rect = {circle_origin , circle_size};

  circle_layer = layer_create(circle_rect);
  layer_add_child(window_layer, circle_layer);
  layer_set_update_proc(circle_layer, circle_layer_draw);

  GRect time_rect = {.origin = {14, 50}, .size = {24 * 4, 60}} ;
  time_layer = layer_create(time_rect);
  layer_add_child(circle_layer, time_layer);
  layer_set_update_proc(time_layer, time_layer_draw);

  tick_timer_service_subscribe(MINUTE_UNIT | HOUR_UNIT, handle_minute_tick);

  digits[0] = gbitmap_create_with_resource(RESOURCE_ID_DIGIT_0);
  digits[1] = gbitmap_create_with_resource(RESOURCE_ID_DIGIT_1);
  digits[2] = gbitmap_create_with_resource(RESOURCE_ID_DIGIT_2);
  digits[3] = gbitmap_create_with_resource(RESOURCE_ID_DIGIT_3);
  digits[4] = gbitmap_create_with_resource(RESOURCE_ID_DIGIT_4);
  digits[5] = gbitmap_create_with_resource(RESOURCE_ID_DIGIT_5);
  digits[6] = gbitmap_create_with_resource(RESOURCE_ID_DIGIT_6);
  digits[7] = gbitmap_create_with_resource(RESOURCE_ID_DIGIT_7);
  digits[8] = gbitmap_create_with_resource(RESOURCE_ID_DIGIT_8);
  digits[9] = gbitmap_create_with_resource(RESOURCE_ID_DIGIT_9);

}

static void window_unload(Window *window) {
  layer_destroy(circle_layer);
  layer_destroy(time_layer);
  gbitmap_destroy(digits[0]);
  gbitmap_destroy(digits[1]);
  gbitmap_destroy(digits[2]);
  gbitmap_destroy(digits[3]);
  gbitmap_destroy(digits[4]);
  gbitmap_destroy(digits[5]);
  gbitmap_destroy(digits[6]);
  gbitmap_destroy(digits[7]);
  gbitmap_destroy(digits[8]);
  gbitmap_destroy(digits[9]);
}

static void init(void) {
  window = window_create();
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  const bool animated = true;
  window_stack_push(window, animated);
}

static void deinit(void) {
  window_destroy(window);
}

int main(void) {
  init();

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", window);

  app_event_loop();
  deinit();
}
