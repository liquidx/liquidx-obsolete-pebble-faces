#include <pebble.h>

// Black on white.
// #define BACKGROUND_COLOR GColorWhite
// #define FOREGROUND_COLOR GColorBlack

// White on black.
#define BACKGROUND_COLOR GColorBlack
#define FOREGROUND_COLOR GColorWhite

#define ANIMATE_DIALS 0
#define DEBUG 0

static Window *window;
static Layer *canvas_layer;
static Layer *dial_layers[12];
static GPoint clear_dial_path_points[7];
static GPathInfo clear_dial_path_info;
static GPath *clear_dial_path = NULL;

static PropertyAnimation *dial_animations[12];

static TextLayer *debug_text_layer;
static char debug_text[64];

int16_t current_hour;
int16_t current_minute;

// Lookup table from 0-59 minutes for the x-y offset
// from the 0-minute position on a square. It's very rough.
// for i in range(0, 15):
//     math.tan((float(i) * 6)/180.0 * 3.1416) * 18
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

// Canvas (3x4 in 144x168)
// static GSize dialSize = {32, 32};
// static int16_t dialRadius = 16;
// static GPoint faceMargin = {14, 8};
// static GPoint dialSeparator = {10, 8};

// Larger
static GSize dialSize = {36, 36};
static int16_t dialRadius = 18;
static GPoint faceMargin = {10, 3};
static GPoint dialSeparator = {8, 6};


typedef struct _DialData {
  int16_t hour;
} DialData;


//
static void update_minute_dial_path(int16_t minute) {
  GPoint dialCenter = { dialRadius, dialRadius };

  // Partial hour dial.
  GPoint dial0 = { dialCenter.x, dialCenter.y - dialRadius };
  GPoint dial30 = { dialCenter.x, dialCenter.y + dialRadius };
  GPoint dial_tr = {dialCenter.x + dialRadius, dialCenter.y - dialRadius};
  GPoint dial_br = {dialCenter.x + dialRadius, dialCenter.y + dialRadius};
  GPoint dial_bl = {dialCenter.x - dialRadius, dialCenter.y + dialRadius};
  GPoint dial_tl = {dialCenter.x - dialRadius, dialCenter.y - dialRadius};

  int16_t minute_x = dial0.x + minute_offset_x_y[minute * 2];
  int16_t minute_y = dial0.y + minute_offset_x_y[minute * 2 + 1];
  GPoint minutePoint = {minute_x, minute_y};

  int16_t num_points = 6;
  if (minute < 13) {
    num_points = 7;
    clear_dial_path_points[0] = dial0;
    clear_dial_path_points[1] = dialCenter;
    clear_dial_path_points[2] = minutePoint;
    clear_dial_path_points[3] = dial_tr;
    clear_dial_path_points[4] = dial_br;
    clear_dial_path_points[5] = dial_bl;
    clear_dial_path_points[6] = dial_tl;
  } else if (minute >= 13 && minute < 23) {
    num_points = 6;
    clear_dial_path_points[0] = dial0;
    clear_dial_path_points[1] = dialCenter;
    clear_dial_path_points[2] = minutePoint;
    clear_dial_path_points[3] = dial_br;
    clear_dial_path_points[4] = dial_bl;
    clear_dial_path_points[5] = dial_tl;
  } else if (minute >= 23 && minute < 37) {
    num_points = 5;
    clear_dial_path_points[0] = dial0;
    clear_dial_path_points[1] = dialCenter;
    clear_dial_path_points[2] = minutePoint;
    clear_dial_path_points[3] = dial_bl;
    clear_dial_path_points[4] = dial_tl;
  } else if (minute >= 38 && minute < 52) {
    num_points = 4;
    clear_dial_path_points[0] = dial0;
    clear_dial_path_points[1] = dialCenter;
    clear_dial_path_points[2] = minutePoint;
    clear_dial_path_points[3] = dial_tl;
  } else if (minute >= 52) {
    num_points = 3;
    clear_dial_path_points[0] = dial0;
    clear_dial_path_points[1] = dialCenter;
    clear_dial_path_points[2] = minutePoint;
  }

  // if (minute < 30) {
  //   num_points = 5;
  //   clear_dial_path_points[0] = dial0;
  //   clear_dial_path_points[1] = dial_tr;
  //   clear_dial_path_points[2] = minutePoint;
  //   clear_dial_path_points[3] = dialCenter;
  //   clear_dial_path_points[4] = dial0;
  // } else {
  //   num_points = 5;
  //   clear_dial_path_points[0] = dial0;
  //   clear_dial_path_points[1] = dialCenter;
  //   clear_dial_path_points[2] = minutePoint;
  //   clear_dial_path_points[3] = dial_tl;
  //   clear_dial_path_points[4] = dial0;
  // }

  clear_dial_path_info.num_points = num_points;
  clear_dial_path_info.points = clear_dial_path_points;
  if (clear_dial_path) {
    gpath_destroy(clear_dial_path);
  }
  clear_dial_path = gpath_create(&clear_dial_path_info);
}

static void dial_canvas_layer_draw(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_bounds(layer);
  graphics_context_set_fill_color(ctx, BACKGROUND_COLOR);
  graphics_fill_rect(ctx, bounds, 0, GCornerNone);

  GPoint dialCenter = { dialRadius, dialRadius };

  // Get time
  int16_t hour = current_hour % 12;
  int16_t minute = current_minute;

  DialData *dial_data = layer_get_data(layer);
  if (dial_data->hour < hour) {
    // Filled in hour dial
    graphics_context_set_fill_color(ctx, FOREGROUND_COLOR);
    graphics_fill_circle(ctx, dialCenter, dialRadius);
    graphics_context_set_stroke_color(ctx, FOREGROUND_COLOR);
    graphics_draw_circle(ctx, dialCenter, dialRadius);
  } else if (dial_data->hour > hour) {
    // Empty hour dial
    graphics_context_set_fill_color(ctx, BACKGROUND_COLOR);
    graphics_fill_circle(ctx, dialCenter, dialRadius);
    graphics_context_set_stroke_color(ctx, FOREGROUND_COLOR);
    graphics_draw_circle(ctx, dialCenter, dialRadius);
  } else {
    graphics_context_set_fill_color(ctx, FOREGROUND_COLOR);
    graphics_fill_circle(ctx, dialCenter, dialRadius);

    if (clear_dial_path) {
      graphics_context_set_fill_color(ctx, BACKGROUND_COLOR);
      gpath_draw_filled(ctx, clear_dial_path);
    }

    graphics_context_set_stroke_color(ctx, FOREGROUND_COLOR);
    graphics_draw_circle(ctx, dialCenter, dialRadius);

#if DEBUG
    memset(debug_text, 0, 64);
    snprintf(debug_text, 64, "%d:%d", hour, minute);
    text_layer_set_text(debug_text_layer, debug_text);
    // Draw stroke outline for debugging
    //graphics_context_set_stroke_color(ctx, FOREGROUND_COLOR);
    //graphics_draw_circle(ctx, dialCenter, dialRadius / 2);
#endif  // DBEUG

  }

}

static void canvas_layer_draw(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_bounds(layer);

  graphics_context_set_fill_color(ctx, BACKGROUND_COLOR);
  graphics_fill_rect(ctx, bounds, 0, GCornerNone);
}

// animations

static void dial_animation_started(Animation *animation, void *data) {
}

static void dial_animation_stopped(Animation *animation, bool finished, void *data) {
  animation_destroy(animation);
}


// time keeping

static void handle_minute_tick(struct tm *tick_time, TimeUnits units_changed) {
  current_hour = tick_time->tm_hour;
  current_minute = tick_time->tm_min;
  update_minute_dial_path(current_minute);

  // Mark all dials for redrawing.
  layer_mark_dirty(canvas_layer);
  for (int index = 0; index < 12; index++) {
    layer_mark_dirty(dial_layers[index]);

#if ANIMATE_DIALS
    // set up animation
    GRect end_frame = layer_get_frame(dial_layers[index]);
    GRect start_frame = end_frame;
    start_frame.origin.x += 10;

    Animation *animation = (Animation *)property_animation_create_layer_frame(dial_layers[index], &start_frame, &end_frame);

    animation_set_handlers(
      (Animation *)animation,
      (AnimationHandlers) {
        .started = (AnimationStartedHandler) dial_animation_started,
        .stopped = (AnimationStoppedHandler) dial_animation_stopped,
      },
      NULL);
    animation_set_delay(animation, index * 10);
    animation_schedule(animation);
    dial_animations[index] = (PropertyAnimation *)animation;
#endif  // ANIMATE_DIALS
  }


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

#if DEBUG
  int16_t debug_text_height = 24;
  debug_text_layer = text_layer_create(
    (GRect){
      //.origin = {0, bounds.size.h - debug_text_height},
      .origin = {0, 0},
      .size = {bounds.size.w, debug_text_height}
    });
  GFont *font = fonts_get_system_font(FONT_KEY_GOTHIC_14);
  text_layer_set_font(debug_text_layer, font);
  layer_add_child(window_layer, (Layer *)debug_text_layer);
#endif  // DEBUG

  const int kRows = 4;
  const int kCols = 3;
  for (int row = 0; row < kRows; row++) {
    for (int col = 0; col < kCols; col++) {
      GRect dial_rect =  {
        .origin = {
          faceMargin.x + col * (dialSize.w + dialSeparator.x),
          faceMargin.y + row * (dialSize.h + dialSeparator.y)
        },
        .size = {
          dialRadius * 2 + 2,
          dialRadius * 2 + 2
        }
      };
      Layer *dial_layer = layer_create_with_data(dial_rect, sizeof(DialData));
      layer_set_update_proc(dial_layer, dial_canvas_layer_draw);

      int16_t index = row * kCols + col;
      DialData *dial_data = layer_get_data(dial_layer);
      dial_data->hour = index;
      layer_add_child(canvas_layer, dial_layer);
      dial_layers[index] = dial_layer;
    }
  }


  tick_timer_service_subscribe(MINUTE_UNIT | HOUR_UNIT, handle_minute_tick);
}

static void window_unload(Window *window) {
  layer_destroy(canvas_layer);
  tick_timer_service_unsubscribe();
  if (clear_dial_path) {
    gpath_destroy(clear_dial_path);
  }
  for (int i = 0; i < 12; i++) {
    layer_destroy(dial_layers[i]);
  }
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
