#include <pebble.h>

#include "./modules/render.h"

#define NUM_BACKGROUND_COLOR_PKEY 1
#define NUM_TEXT_COLOR_PKEY 2
#define NUM_WEATHER_PKEY 3

GColor background_color;
GColor text_color;
char weather_layer_buffer[32];

static Window *s_main_window;
static bool is_connected;

static void send_command(char *command) {
  // Declare the dictionary's iterator
  DictionaryIterator *out_iter;
  
  // Prepare the outbox buffer for this message
  AppMessageResult result = app_message_outbox_begin(&out_iter);
  if(result == APP_MSG_OK) {
    // Add an item to ask for weather data
    dict_write_cstring (out_iter, MESSAGE_KEY_COMMAND, command);
  
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Sent message: %s", command);
    
    // Send this message
    result = app_message_outbox_send();
    if(result != APP_MSG_OK) {
      APP_LOG(APP_LOG_LEVEL_ERROR, "Error sending the outbox: %d", (int)result);
    } 
  } else {
    // The outbox cannot be used right now
    APP_LOG(APP_LOG_LEVEL_ERROR, "Error preparing the outbox: %d", (int)result);
  }
}

static void bluetooth_callback(bool connected) {
  toggle_bt_icon(connected);

  if(!connected) {
    vibes_double_pulse();
  } else if (!is_connected) {
    vibes_short_pulse();
  }

  is_connected = connected;
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
  update_time_data(s_time_buffer);
  update_date_data(s_date_buffer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
  
  // Get weather update every 30 minutes
  if(tick_time->tm_min % 30 == 0) {
    send_command("weather");
  }
}

static void main_window_load(Window *window) {
  
  // Get information about the Window
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  set_background_color(background_color);
  set_text_color(text_color);

  draw_time(bounds, window_layer);
  draw_date(bounds, window_layer);
  draw_weather(bounds, window_layer, weather_layer_buffer);
  draw_lines(bounds, window_layer);
  
  create_bt_icon(bounds, window_layer);
  bluetooth_callback(connection_service_peek_pebble_app_connection());
}

static void main_window_unload(Window *window) {

  render_destroy();

}

static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
  static char temperature_buffer[8];
  static char conditions_buffer[32];
  static char weather_layer_buffer[32];
  
  // Read tuples for data
  Tuple *temp_tuple = dict_find(iterator, MESSAGE_KEY_TEMPERATURE);
  Tuple *conditions_tuple = dict_find(iterator, MESSAGE_KEY_CONDITIONS);
  Tuple *bg_color_t = dict_find(iterator, MESSAGE_KEY_BACKGROUND_COLOR);
  Tuple *txt_color_t = dict_find(iterator, MESSAGE_KEY_TEXT_COLOR);
  
  // If all data is available, use it
  if(temp_tuple && conditions_tuple) {
    snprintf(temperature_buffer, sizeof(temperature_buffer), "%d ºc", (int)temp_tuple->value->int32);
    snprintf(conditions_buffer, sizeof(conditions_buffer), "%s", conditions_tuple->value->cstring);
    
    // Assemble full string and display
    snprintf(weather_layer_buffer, sizeof(weather_layer_buffer), "%s, %s", temperature_buffer, conditions_buffer);
    update_weather_data(weather_layer_buffer);
  }

  if(bg_color_t) {
    GColor background_color = GColorFromHEX(bg_color_t->value->int32);

    window_set_background_color(s_main_window, background_color);
    update_background_color(background_color);  
  }
  
  if(txt_color_t) {
    GColor text_color = GColorFromHEX(txt_color_t->value->int32);
    
    update_text_color(text_color);
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
  is_connected = true;

  if (persist_exists(NUM_BACKGROUND_COLOR_PKEY)) {
    persist_read_data(NUM_BACKGROUND_COLOR_PKEY, &background_color, sizeof(GColor));    
  } else {
    background_color = GColorWhite;
  }

  if (persist_exists(NUM_TEXT_COLOR_PKEY)) {
    persist_read_data(NUM_TEXT_COLOR_PKEY, &text_color, sizeof(GColor));    
  } else {
    background_color = GColorBlack;
  }

  if (persist_exists(NUM_WEATHER_PKEY)) {
    persist_read_string(NUM_WEATHER_PKEY, weather_layer_buffer, 32);
  } else {
    strcpy (weather_layer_buffer,"Loading...");
  }

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
  persist_write_string(NUM_WEATHER_PKEY, weather_layer_buffer);
  persist_write_data(NUM_BACKGROUND_COLOR_PKEY, &background_color, sizeof(background_color));
  persist_write_data(NUM_TEXT_COLOR_PKEY, &text_color, sizeof(text_color));

  // Destroy Window
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}