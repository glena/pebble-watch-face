#include "render.h"

static TextLayer *s_time_layer;
static TextLayer *s_date_layer;
static GFont s_time_font;
static GFont s_date_font;
static GBitmap *s_bt_icon_bitmap;
static Layer *s_canvas_layer;
static TextLayer *s_weather_layer;
static BitmapLayer *s_bt_icon_layer;

static GColor background_color_default = GColorWhite;
static GColor text_color_default = GColorBlack;

static void update_time_data(char *s_time_buffer) {
  text_layer_set_text(s_time_layer, s_time_buffer);
}

static void update_date_data(char *s_date_buffer) {
  text_layer_set_text(s_date_layer, s_date_buffer);
}

static void update_weather_data(char *weather_layer_buffer) {
  text_layer_set_text(s_weather_layer, weather_layer_buffer);
}

static void toggle_bt_icon(bool connected) {
  layer_set_hidden(bitmap_layer_get_layer(s_bt_icon_layer), connected);
}

static void draw_time(GRect bounds, Layer *window_layer) {
  s_time_layer = text_layer_create(GRect(0, PBL_IF_ROUND_ELSE(62, 56), bounds.size.w, 50));
  s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ROBOTO_42));
  text_layer_set_font(s_time_layer, s_time_font);
  
  text_layer_set_background_color(s_time_layer, background_color_default);
  text_layer_set_text_color(s_time_layer, text_color_default);
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
  
  layer_add_child(window_layer, text_layer_get_layer(s_time_layer));
}

static void draw_date(GRect bounds, Layer *window_layer) {
  s_date_layer = text_layer_create(GRect(0, 10, bounds.size.w, 20));
  s_date_font = fonts_load_custom_font(fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
  text_layer_set_font(s_date_layer, s_date_font);
  
  text_layer_set_background_color(s_date_layer, background_color_default);
  text_layer_set_text_color(s_date_layer, text_color_default);
  text_layer_set_text_alignment(s_date_layer, GTextAlignmentCenter);
  
  layer_add_child(window_layer, text_layer_get_layer(s_date_layer));
}

static void canvas_update_proc(Layer *layer, GContext *ctx) {

  GRect bounds = layer_get_bounds(layer);
  
  graphics_context_set_stroke_color(ctx, text_color_default);
  graphics_context_set_fill_color(ctx, text_color_default);
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

static void draw_lines(GRect bounds, Layer *window_layer) {
  s_canvas_layer = layer_create(bounds);
  
  layer_set_update_proc(s_canvas_layer, canvas_update_proc);
  
  layer_add_child(window_layer, s_canvas_layer);
}

static void draw_weather(GRect bounds, Layer *window_layer) {
  s_weather_layer = text_layer_create(GRect(0, bounds.size.h - 30, bounds.size.w, 20));
  
  text_layer_set_background_color(s_weather_layer, background_color_default);
  text_layer_set_text_color(s_weather_layer, text_color_default);
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
}

static void render_destroy() {
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

static void update_background_color(GColor background_color) {
  text_layer_set_background_color(s_time_layer, background_color);
  text_layer_set_background_color(s_date_layer, background_color);
  text_layer_set_background_color(s_weather_layer, background_color);
}

static void update_text_color(GColor text_color) {
  text_layer_set_text_color(s_time_layer, text_color);
  text_layer_set_text_color(s_date_layer, text_color);
  text_layer_set_text_color(s_weather_layer, text_color);
}