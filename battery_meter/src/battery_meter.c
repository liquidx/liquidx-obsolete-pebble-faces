#include <pebble.h>

static Window *window;
static TextLayer *text_layer;
static char battery_string[64];

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  text_layer_set_text(text_layer, "Select");
}


static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
}

static void window_load(Window *window) {
  window_set_background_color(window, GColorBlack);
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  GRect text_frame = {
    .origin = { 0, (bounds.size.h - 50) / 2},
    .size = { bounds.size.w, 50}
  };
  text_layer = text_layer_create(text_frame);
  GFont *font = fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD);
  text_layer_set_font(text_layer, font);
  text_layer_set_text_color(text_layer, GColorWhite);
  text_layer_set_background_color(text_layer, GColorBlack);
  text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(text_layer));

  // Set battery.
  BatteryChargeState battery_state = battery_state_service_peek();
  memset(battery_string, 0, 64);
  snprintf(battery_string, 64, "%d%%", battery_state.charge_percent);
  text_layer_set_text(text_layer, battery_string);

}

static void window_unload(Window *window) {
  text_layer_destroy(text_layer);
}

static void init(void) {
  window = window_create();
  window_set_click_config_provider(window, click_config_provider);
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
