#pragma once
#include <pebble.h>

typedef struct RenderResources {
  TextLayer *s_time_layer;
  TextLayer *s_date_layer;
  TextLayer *s_weather_layer;

  BitmapLayer *s_bt_icon_layer;

  Layer *s_canvas_layer;

  GFont s_primary_font;
  GFont s_secondary_font;

  GBitmap *s_bt_icon_bitmap;
} RenderResources;

struct RenderResources* render_init(GRect bounds);

void update_time_data(RenderResources *resources, char *s_time_buffer);

void update_date_data(RenderResources *resources, char *s_date_buffer);

void update_weather_data(RenderResources *resources, char *weather_layer_buffer);

void toggle_bt_icon(RenderResources *resources, bool connected);

void draw_time(RenderResources *resources, GRect bounds, Layer *window_layer, GColor background_color, GColor text_color);

void draw_date(RenderResources *resources, GRect bounds, Layer *window_layer, GColor background_color, GColor text_color);

void draw_lines(RenderResources *resources, GRect bounds, Layer *window_layer, GColor text_color);

void draw_weather(RenderResources *resources, GRect bounds, Layer *window_layer, char *weather_layer_buffer, GColor background_color, GColor text_color);

void create_bt_icon(RenderResources *resources, GRect bounds, Layer *window_layer);

void render_destroy(RenderResources *resources);

void update_background_color(RenderResources *resources, GColor background_color);

void update_text_color(RenderResources *resources, GColor text_color);
