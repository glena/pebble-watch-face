#include <pebble.h>

static Window *s_main_window;
static TextLayer *s_time_layer;
static GFont s_time_font;
static Layer *s_canvas_layer;

static void update_time() {
  // Get a tm structure
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);

  // Write the current hours and minutes into a buffer
  static char s_buffer[8];
  strftime(s_buffer, sizeof(s_buffer), clock_is_24h_style() ?
                                          "%H:%M" : "%I:%M", tick_time);

  // Display this time on the TextLayer
  text_layer_set_text(s_time_layer, s_buffer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}

static void canvas_update_proc(Layer *layer, GContext *ctx) {

  GRect bounds = layer_get_bounds(layer);
  
  // Set the line color
  graphics_context_set_stroke_color(ctx, GColorWhite);
  
  // Set the fill color
  graphics_context_set_fill_color(ctx, GColorWhite);
    
    
  // Set the stroke width (must be an odd integer value)
  graphics_context_set_stroke_width(ctx, 1);
  
  GPoint start_1 = GPoint(25, PBL_IF_ROUND_ELSE(67, 61));
  GPoint end_1 = GPoint(bounds.size.w - 25, PBL_IF_ROUND_ELSE(67, 61));
  
  graphics_draw_line(ctx, start_1, end_1);
  
  GPoint start_2 = GPoint(25, PBL_IF_ROUND_ELSE(124, 118));
  GPoint end_2 = GPoint(bounds.size.w - 25, PBL_IF_ROUND_ELSE(124, 118));
  
  graphics_draw_line(ctx, start_2, end_2);

}

static void main_window_load(Window *window) {
  // Get information about the Window
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  // Create the TextLayer with specific bounds
  s_time_layer = text_layer_create(GRect(0, PBL_IF_ROUND_ELSE(70, 64), bounds.size.w, 50));
  
  // Create canvas layer
  s_canvas_layer = layer_create(bounds);
  
  // Create GFont
  s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_REPET_42));
  
  // Apply to TextLayer
  text_layer_set_font(s_time_layer, s_time_font);
  //   text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));

  // Improve the layout to be more like a watchface
  text_layer_set_background_color(s_time_layer, GColorBlack);
  text_layer_set_text_color(s_time_layer, GColorWhite);
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);

  // Add it as a child layer to the Window's root layer
  layer_add_child(window_layer, text_layer_get_layer(s_time_layer));
  
  // Assign the custom drawing procedure
  layer_set_update_proc(s_canvas_layer, canvas_update_proc);
  
  // Add to Window
  layer_add_child(window_get_root_layer(window), s_canvas_layer);
}

static void main_window_unload(Window *window) {
  // Destroy TextLayer
  text_layer_destroy(s_time_layer);
  
  // Destroy CanvasLayer
  layer_destroy(s_canvas_layer);
  
  // Unload GFont
  fonts_unload_custom_font(s_time_font);
}

static void init() {
  // Create main Window element and assign to pointer
  s_main_window = window_create();
  
  window_set_background_color(s_main_window, GColorBlack);

  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

  // Show the Window on the watch, with animated=true
  window_stack_push(s_main_window, true);
  
  // Register with TickTimerService
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  update_time();
}

static void deinit() {
  // Destroy Window
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}