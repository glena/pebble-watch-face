#pragma once
#include <pebble.h>

void update_time_data(char *s_time_buffer);

void update_date_data(char *s_date_buffer);

void update_weather_data(char *weather_layer_buffer);

void toggle_bt_icon(bool connected);

void draw_time(GRect bounds, Layer *window_layer);

void draw_date(GRect bounds, Layer *window_layer);

void draw_lines(GRect bounds, Layer *window_layer);

void draw_weather(GRect bounds, Layer *window_layer);

void create_bt_icon(GRect bounds, Layer *window_layer);

void render_destroy();

void update_background_color(GColor background_color);

void update_text_color(GColor text_color);

void set_background_color(GColor new_background_color);
void set_text_color(GColor new_text_color);