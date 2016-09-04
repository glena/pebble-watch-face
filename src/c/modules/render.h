#pragma once
#include <pebble.h>

static void update_time_data(char *s_time_buffer);

static void update_date_data(char *s_date_buffer);

static void update_weather_data(char *weather_layer_buffer);

static void toggle_bt_icon(bool connected);

static void draw_time(GRect bounds, Layer *window_layer);

static void draw_date(GRect bounds, Layer *window_layer);

static void draw_lines(GRect bounds, Layer *window_layer);

static void draw_weather(GRect bounds, Layer *window_layer);

static void create_bt_icon(GRect bounds, Layer *window_layer);

static void render_destroy();

static void update_background_color(GColor background_color);

static void update_text_color(GColor text_color);