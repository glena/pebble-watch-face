#include <pebble.h>

static Window *s_main_window;
static TextLayer *s_time_layer;
static TextLayer *s_date_layer;
static GFont s_time_font;
static GFont s_date_font;
static Layer *s_canvas_layer;
static TextLayer *s_weather_layer;
static BitmapLayer *s_bt_icon_layer;
static GBitmap *s_bt_icon_bitmap;
static GColor background_color;
static GColor text_color;

static void bluetooth_callback(bool connected) {
  // Show icon if disconnected
  layer_set_hidden(bitmap_layer_get_layer(s_bt_icon_layer), connected);

  if(!connected) {
    vibes_double_pulse();
  } else {
    vibes_short_pulse();
  }
}

static void update_time() {
  // Get a tm structure
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);

  // Write the current hours and minutes into a buffer
  static char s_time_buffer[8];
  strftime(s_time_buffer, sizeof(s_time_buffer), clock_is_24h_style() ? "%H:%M" : "%I:%M", tick_time);
  
  static char s_date_buffer[11];
  strftime(s_date_buffer, sizeof(s_date_buffer), "%a %b %e", tick_time);

  // Display this time on the TextLayer
  text_layer_set_text(s_time_layer, s_time_buffer);
  text_layer_set_text(s_date_layer, s_date_buffer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
  
  // Get weather update every 30 minutes
  if(tick_time->tm_min % 30 == 0) {
    // Begin dictionary
    DictionaryIterator *iter;
    app_message_outbox_begin(&iter);
  
    // Add a key-value pair
    dict_write_uint8(iter, 0, 0);
  
    // Send the message!
    app_message_outbox_send();
  }
}

static void canvas_update_proc(Layer *layer, GContext *ctx) {

  GRect bounds = layer_get_bounds(layer);
  
  graphics_context_set_stroke_color(ctx, text_color);
  graphics_context_set_fill_color(ctx, text_color);
  graphics_context_set_stroke_width(ctx, 1);
  
  int first_line_top = PBL_IF_ROUND_ELSE(67, 61);
  int time_height = 43;
  int side_margin = 25;
  
  GPoint start_1 = GPoint(side_margin, first_line_top);
  GPoint end_1 = GPoint(bounds.size.w - side_margin - 2, first_line_top);
  
  GPoint start_2 = GPoint(side_margin, first_line_top + time_height);
  GPoint end_2 = GPoint(bounds.size.w - side_margin - 2, first_line_top + time_height);
  
  graphics_draw_line(ctx, start_1, end_1);
  graphics_draw_line(ctx, start_2, end_2);

}

static void draw_time(GRect bounds, Layer *window_layer) {
  s_time_layer = text_layer_create(GRect(0, PBL_IF_ROUND_ELSE(70, 64), bounds.size.w, 50));
  s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_DOT_FONT_32));
  text_layer_set_font(s_time_layer, s_time_font);
  
  text_layer_set_background_color(s_time_layer, background_color);
  text_layer_set_text_color(s_time_layer, text_color);
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
  
  layer_add_child(window_layer, text_layer_get_layer(s_time_layer));
}

static void draw_date(GRect bounds, Layer *window_layer) {
  s_date_layer = text_layer_create(GRect(0, 10, bounds.size.w, 20));
  s_date_font = fonts_load_custom_font(fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
  text_layer_set_font(s_date_layer, s_date_font);
  
  text_layer_set_background_color(s_date_layer, background_color);
  text_layer_set_text_color(s_date_layer, text_color);
  text_layer_set_text_alignment(s_date_layer, GTextAlignmentCenter);
  
  layer_add_child(window_layer, text_layer_get_layer(s_date_layer));
}

static void draw_lines(GRect bounds, Layer *window_layer) {
  s_canvas_layer = layer_create(bounds);
  
  layer_set_update_proc(s_canvas_layer, canvas_update_proc);
  
  layer_add_child(window_layer, s_canvas_layer);
}

static void draw_weather(GRect bounds, Layer *window_layer) {
  s_weather_layer = text_layer_create(GRect(0, bounds.size.h - 30, bounds.size.w, 20));
  
  text_layer_set_background_color(s_weather_layer, background_color);
  text_layer_set_text_color(s_weather_layer, text_color);
  text_layer_set_text_alignment(s_weather_layer, GTextAlignmentCenter);
  text_layer_set_text(s_weather_layer, "Loading...");
  text_layer_set_font(s_weather_layer, s_date_font);
  
  layer_add_child(window_layer, text_layer_get_layer(s_weather_layer));
}

static void create_bt_icon(GRect bounds, Layer *window_layer) {
  // Create the Bluetooth icon GBitmap
  s_bt_icon_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BT_ICON);
  
  // Create the BitmapLayer to display the GBitmap
  s_bt_icon_layer = bitmap_layer_create(GRect(bounds.size.w - 40, 12, 30, 30));
  bitmap_layer_set_bitmap(s_bt_icon_layer, s_bt_icon_bitmap);
  layer_add_child(window_layer, bitmap_layer_get_layer(s_bt_icon_layer));
  
  bluetooth_callback(connection_service_peek_pebble_app_connection());
}

static void main_window_load(Window *window) {
  // Get information about the Window
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  
  draw_time(bounds, window_layer);
  draw_date(bounds, window_layer);
  draw_weather(bounds, window_layer);
  draw_lines(bounds, window_layer);
  
  create_bt_icon(bounds, window_layer);
}

static void main_window_unload(Window *window) {
  // Destroy TextLayer
  text_layer_destroy(s_time_layer);
  text_layer_destroy(s_date_layer);
  text_layer_destroy(s_weather_layer);
  
  // Destroy CanvasLayer
  layer_destroy(s_canvas_layer);
  
  // Unload GFont
  fonts_unload_custom_font(s_time_font);
  fonts_unload_custom_font(s_date_font);
  
  // unload bt icon
  gbitmap_destroy(s_bt_icon_bitmap);
  bitmap_layer_destroy(s_bt_icon_layer);
}

static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
  static char temperature_buffer[8];
  static char conditions_buffer[32];
  static char weather_layer_buffer[32];
  
  // Read tuples for data
  Tuple *temp_tuple = dict_find(iterator, MESSAGE_KEY_TEMPERATURE);
  Tuple *conditions_tuple = dict_find(iterator, MESSAGE_KEY_CONDITIONS);
  
  // If all data is available, use it
  if(temp_tuple && conditions_tuple) {
    snprintf(temperature_buffer, sizeof(temperature_buffer), "%dC", (int)temp_tuple->value->int32);
    snprintf(conditions_buffer, sizeof(conditions_buffer), "%s", conditions_tuple->value->cstring);
    
    // Assemble full string and display
    snprintf(weather_layer_buffer, sizeof(weather_layer_buffer), "%s, %s", temperature_buffer, conditions_buffer);
    text_layer_set_text(s_weather_layer, weather_layer_buffer);
  }
}

static void inbox_dropped_callback(AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}

static void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}

static void init() {
  background_color = GColorWhite;
  text_color = GColorBlack;
  
  // Create main Window element and assign to pointer
  s_main_window = window_create();
  
  window_set_background_color(s_main_window, background_color);

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
  
  app_message_register_inbox_received(inbox_received_callback);
  app_message_register_inbox_dropped(inbox_dropped_callback);
  app_message_register_outbox_failed(outbox_failed_callback);
  app_message_register_outbox_sent(outbox_sent_callback);
  
  // Open AppMessage
  const int inbox_size = 128;
  const int outbox_size = 128;
  app_message_open(inbox_size, outbox_size);
  
  // Register for Bluetooth connection updates
  connection_service_subscribe((ConnectionHandlers) {
    .pebble_app_connection_handler = bluetooth_callback
  });
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