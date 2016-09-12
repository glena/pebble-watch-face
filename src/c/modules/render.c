#include "render.h"

RenderResources* render_init(GRect bounds) {
  RenderResources *resources;
  resources = malloc(sizeof(RenderResources)); 

  resources->s_time_layer = text_layer_create(GRect(0, PBL_IF_ROUND_ELSE(56, 50), bounds.size.w, 50));
  resources->s_date_layer = text_layer_create(GRect(0, 10, bounds.size.w, 20));
  resources->s_weather_layer = text_layer_create(GRect(0, bounds.size.h - 30, bounds.size.w, 20));
  resources->s_canvas_layer = layer_create(bounds);
  resources->s_bt_icon_layer = bitmap_layer_create(GRect(bounds.size.w - 40, 12, 30, 30));

  resources->s_primary_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_PENDULE_48));
  // resources->s_primary_font = fonts_get_system_font(FONT_KEY_LECO_32_BOLD_NUMBERS);
  resources->s_secondary_font = fonts_get_system_font(FONT_KEY_GOTHIC_18);

  resources->s_bt_icon_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BT_ICON);

  return resources;
}

void update_time_data(RenderResources *resources, char *s_time_buffer) {
  text_layer_set_text(resources->s_time_layer, s_time_buffer);
}

void update_date_data(RenderResources *resources, char *s_date_buffer) {
  text_layer_set_text(resources->s_date_layer, s_date_buffer);
}

void update_weather_data(RenderResources *resources, char *weather_layer_buffer) {
  text_layer_set_text(resources->s_weather_layer, weather_layer_buffer);
}

void toggle_bt_icon(RenderResources *resources, bool connected) {
  layer_set_hidden(bitmap_layer_get_layer(resources->s_bt_icon_layer), connected);
}

void draw_time(RenderResources *resources, GRect bounds, Layer *window_layer, GColor background_color, GColor text_color) {
  text_layer_set_font(resources->s_time_layer, resources->s_primary_font);
  
  text_layer_set_background_color(resources->s_time_layer, background_color);
  text_layer_set_text_color(resources->s_time_layer, text_color);
  text_layer_set_text_alignment(resources->s_time_layer, GTextAlignmentCenter);
  
  //TODO Move this
  layer_add_child(window_layer, text_layer_get_layer(resources->s_time_layer));
}

void draw_date(RenderResources *resources, GRect bounds, Layer *window_layer, GColor background_color, GColor text_color) {
  text_layer_set_font(resources->s_date_layer, resources->s_secondary_font);
  
  text_layer_set_background_color(resources->s_date_layer, background_color);
  text_layer_set_text_color(resources->s_date_layer, text_color);
  text_layer_set_text_alignment(resources->s_date_layer, GTextAlignmentCenter);
  
  layer_add_child(window_layer, text_layer_get_layer(resources->s_date_layer));
}

GColor lines_color;
void canvas_update_proc(Layer *layer, GContext *ctx) {

  GRect bounds = layer_get_bounds(layer);
  
  graphics_context_set_stroke_color(ctx, lines_color);
  graphics_context_set_fill_color(ctx, lines_color);
  graphics_context_set_stroke_width(ctx, 1);
  
  int first_line_top = PBL_IF_ROUND_ELSE(67, 61);
  int time_height = 43;
  int side_margin = 27;
  
  GPoint start_1 = GPoint(side_margin, first_line_top);
  GPoint end_1 = GPoint(bounds.size.w - side_margin - 2, first_line_top);
  
  GPoint start_2 = GPoint(side_margin, first_line_top + time_height);
  GPoint end_2 = GPoint(bounds.size.w - side_margin - 2, first_line_top + time_height);
  
  graphics_draw_line(ctx, start_1, end_1);
  graphics_draw_line(ctx, start_2, end_2);

}

void draw_lines(RenderResources *resources, GRect bounds, Layer *window_layer, GColor text_color) {
  lines_color = text_color;
  
  layer_set_update_proc(resources->s_canvas_layer, canvas_update_proc);
  
  layer_add_child(window_layer, resources->s_canvas_layer);
}

void draw_weather(RenderResources *resources, GRect bounds, Layer *window_layer, char *weather_layer_buffer, GColor background_color, GColor text_color) {

  text_layer_set_background_color(resources->s_weather_layer, background_color);
  text_layer_set_text_color(resources->s_weather_layer, text_color);
  text_layer_set_text_alignment(resources->s_weather_layer, GTextAlignmentCenter);
  text_layer_set_text(resources->s_weather_layer, weather_layer_buffer);
  text_layer_set_font(resources->s_weather_layer, resources->s_secondary_font);
  
  layer_add_child(window_layer, text_layer_get_layer(resources->s_weather_layer));
}

void create_bt_icon(RenderResources *resources, GRect bounds, Layer *window_layer) {
  bitmap_layer_set_bitmap(resources->s_bt_icon_layer, resources->s_bt_icon_bitmap);
  layer_add_child(window_layer, bitmap_layer_get_layer(resources->s_bt_icon_layer));
}

void render_destroy(RenderResources *resources) {
  // Destroy TextLayer
  text_layer_destroy(resources->s_time_layer);
  text_layer_destroy(resources->s_date_layer);
  text_layer_destroy(resources->s_weather_layer);
  
  // Destroy CanvasLayer
  layer_destroy(resources->s_canvas_layer);
  
  // Unload GFont
  fonts_unload_custom_font(resources->s_primary_font);
  // fonts_unload_custom_font(resources->s_secondary_font);
  
  // unload bt icon
  gbitmap_destroy(resources->s_bt_icon_bitmap);
  bitmap_layer_destroy(resources->s_bt_icon_layer);

  free(resources);
}

void update_background_color(RenderResources *resources, GColor background_color) {
  text_layer_set_background_color(resources->s_time_layer, background_color);
  text_layer_set_background_color(resources->s_date_layer, background_color);
  text_layer_set_background_color(resources->s_weather_layer, background_color);
}

void update_text_color(RenderResources *resources, GColor text_color) {
  text_layer_set_text_color(resources->s_time_layer, text_color);
  text_layer_set_text_color(resources->s_date_layer, text_color);
  text_layer_set_text_color(resources->s_weather_layer, text_color);
}