#pragma once

#define GAME_NAME "COUNTDOWN"

typedef struct STRUCT_WINDOW_DATA
{
    uint32_t id;
    uint32_t width  = 800;
    uint32_t height = 600;
    uint32_t target_fps = 60;
    float    delta_time = 0.0f;
    double   time = 0.0f;
    uint32_t actual_fps = 0;
} WindowData;

extern WindowData g_window_data;