#include <pebble.h>

static Window *window;
static Layer *canvas_layer;
int16_t current_hour;
int16_t current_minute;

// Lookup table for minute to x-value
// for i in range(0, 15):
//     math.tan((float(i) * 6)/180.0 * 3.1416) * 18
// 0.0
// 1.8918806913214514
// 3.826027324049155
// 5.848569151780266
// 8.014137461991712
// 10.392334231074862
// 13.077805911677284
// 16.207328667302942
// 19.991104025086436
// 24.77498939253651
// 31.177090850832574
// 40.428955016319165
// 55.398857588930746
// 84.68466758882202
// 171.2642082385304
static const int16_t minute_offset_x_y[120] = {
  0, 0,

  // 1 .. 5
  2, 0,
  4, 0,
  6, 0,
  8, 0,
  10, 0,

  // 6 .. 10
  13, 0,
  16, 0,
  18, 2,
  18, 4,
  18, 6,

  // 10 .. 15
  18, 8,
  18, 10,
  18, 13,
  18, 16,
  18, 18,

  // 16 .. 20
  18, 20,
  18, 22,
  18, 24,
  18, 26,
  18, 28,

  // 21 .. 25
  18, 31,
  18, 34,
  16, 36,
  13, 36,
  10, 36,

  // 26 .. 30
  8, 36,
  6, 36,
  4, 36,
  2, 36,
  0, 36,

  // 31 .. 35
  -2, 36,
  -4, 36,
  -6, 36,
  -8, 36,
  -10, 36,

  // 36 .. 40
  -13, 36,
  -16, 36,
  -18, 34,
  -18, 32,
  -18, 30,

  // 41 .. 45
  -18, 28,
  -18, 26,
  -18, 23,
  -18, 20,
  -18, 18,

  // 46 .. 50
  -18, 15,
  -18, 12,
  -18, 10,
  -18, 8,
  -18, 6,

  // 51 .. 55
  -18, 4,
  -18, 2,
  -15, 0,
  -12, 0,
  -10, 0,

  // 55 .. 59
  -8, 0,
  -6, 0,
  -4, 0,
  -2, 0
};


// canvas
static void canvas_layer_draw(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_bounds(layer);

  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_fill_rect(ctx, bounds, 0, GCornerNone);

  // Get time
  int16_t hour = current_hour % 12;
  int16_t minute = current_minute;

  // Draw dials
  GSize dialSize = {36, 36};
  int16_t radius = 18;
  GPoint padding = {14, 6};
  GPoint dialSeparator = {6, 4};

  const int kRows = 4;
  const int kCols = 3;
  for (int row = 0; row < kRows; row++) {
    for (int col = 0; col < kCols; col++) {
        GPoint dialCenter = {
          padding.x + col * (dialSize.w + dialSeparator.x) + radius,
          padding.y + row * (dialSize.h + dialSeparator.y) + radius
        };
        GPoint dial0 = { dialCenter.x, dialCenter.y - radius };
        GPoint dial_tr = {dialCenter.x + radius, dialCenter.y - radius};
        GPoint dial_br = {dialCenter.x + radius, dialCenter.y + radius};
        GPoint dial_bl = {dialCenter.x - radius, dialCenter.y + radius};
        GPoint dial_tl = {dialCenter.x - radius, dialCenter.y - radius};

        int dialHour = row * kCols + col;
        if (dialHour < hour) {
          graphics_context_set_fill_color(ctx, GColorWhite);
          graphics_fill_circle(ctx, dialCenter, radius);
          graphics_context_set_stroke_color(ctx, GColorWhite);
          graphics_draw_circle(ctx, dialCenter, radius);
        } else if (dialHour == hour) {
          // Fill circle with white.
          if (minute > 0) {
            graphics_context_set_fill_color(ctx, GColorWhite);
            graphics_fill_circle(ctx, dialCenter, radius);
          }

          // White out the non-filled out part of the circle.
          GPathInfo clock_erase_path = {
            .num_points = 7,
            .points = (GPoint[]) {dial0, dialCenter, dial0, dial0, dial0, dial0, dial0}
          };
          int16_t minute_x = dial0.x + minute_offset_x_y[minute * 2];
          int16_t minute_y = dial0.y + minute_offset_x_y[minute * 2 + 1];
          GPoint minutePoint = {minute_x, minute_y};

          if (minute > 0 && minute < 8) {
            clock_erase_path.points[2] = minutePoint;
            //
            clock_erase_path.points[3] = dial_tr;
            clock_erase_path.points[4] = dial_br;
            clock_erase_path.points[5] = dial_bl;
            clock_erase_path.points[6] = dial_tl;

          } else if (minute >= 8 && minute < 23) {
            clock_erase_path.points[2] = minutePoint;
            clock_erase_path.points[3] = dial_br;
            clock_erase_path.points[4] = dial_bl;
            clock_erase_path.points[5] = dial_tl;

          } else if (minute >= 23 && minute < 38) {
            clock_erase_path.points[2] = minutePoint;
            clock_erase_path.points[3] = dial_bl;
            clock_erase_path.points[4] = dial_tl;

          } else if (minute >= 38 && minute < 53) {
            clock_erase_path.points[2] = minutePoint;
            clock_erase_path.points[3] = dial_tl;
          } else if (minute >= 53) {
            clock_erase_path.points[2] = minutePoint;
          }

          // Clip filled area
          GPath *clock_erase_path_ref = NULL;
          clock_erase_path_ref = gpath_create(&clock_erase_path);
          graphics_context_set_fill_color(ctx, GColorBlack);
          gpath_draw_filled(ctx, clock_erase_path_ref);
          gpath_destroy(clock_erase_path_ref);

          // Draw stroke outline.
          graphics_context_set_stroke_color(ctx, GColorWhite);
          graphics_draw_circle(ctx, dialCenter, radius);
        } else {
          graphics_context_set_fill_color(ctx, GColorBlack);
          graphics_fill_circle(ctx, dialCenter, radius);
          graphics_context_set_stroke_color(ctx, GColorWhite);
          graphics_draw_circle(ctx, dialCenter, radius);
        }
    }
  }
}

// time keeping

static void handle_minute_tick(struct tm *tick_time, TimeUnits units_changed) {
  current_hour = tick_time->tm_hour;
  current_minute = tick_time->tm_min;
  layer_mark_dirty(canvas_layer);
  if (units_changed & HOUR_UNIT) {
    vibes_double_pulse();
  }
}

// window

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  canvas_layer = layer_create((GRect){.origin = {0, 0}, .size = {bounds.size.w, bounds.size.h}});

  layer_add_child(window_layer, canvas_layer);
  layer_set_update_proc(canvas_layer, canvas_layer_draw);
  tick_timer_service_subscribe(MINUTE_UNIT | HOUR_UNIT, handle_minute_tick);
}

static void window_unload(Window *window) {
  layer_destroy(canvas_layer);
  tick_timer_service_unsubscribe();
}

static void init(void) {
  current_hour = 0;
  current_minute = 0;

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
