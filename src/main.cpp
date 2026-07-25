#include "raylib.h"

#include <string>
#include <vector>
#include <sstream>

#include "common.h"
#include "countdown/settings.h"
#include "countdown/game.h"

WindowData g_window_data;
GameData g_game;

void update_user_inputs(GameData &game)
{
    game.inputs.mouse_position      = GetMousePosition();
    game.inputs.mouse_left_pressed  = IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
    game.inputs.mouse_right_pressed = IsMouseButtonPressed(MOUSE_RIGHT_BUTTON);

    game.inputs.kb_grab_pressed       = IsKeyPressed(KEY_G);
    game.inputs.kb_paused_pressed     = IsKeyPressed(KEY_SPACE);
    game.inputs.kb_move_up_pressed    = IsKeyDown(KEY_W);
    game.inputs.kb_move_down_pressed  = IsKeyDown(KEY_S);
    game.inputs.kb_move_left_pressed  = IsKeyDown(KEY_A);
    game.inputs.kb_move_right_pressed = IsKeyDown(KEY_D);

    // TODO : Support Gamepad Inputs
    // TODO : Handle handover of keyboard vs gampead.
}

int main(void)
{
    InitWindow(g_window_data.width, g_window_data.height, GAME_NAME);

    g_window_data.id     = GetCurrentMonitor();
    g_window_data.width  = GetMonitorWidth(g_window_data.id);
    g_window_data.height = GetMonitorHeight(g_window_data.id);

    HideCursor();

    if (!IsWindowFullscreen())
    {
        ToggleFullscreen();
    }

    if(g_settings.enable_vsync)
    {
        SetWindowState(FLAG_VSYNC_HINT);
    }
    else 
    {
        SetTargetFPS(g_window_data.target_fps);
    }

    game_init(g_game);

    while (!WindowShouldClose())
    {
        g_window_data.delta_time = GetFrameTime();
        g_window_data.time       = GetTime();
        g_window_data.actual_fps = GetFPS();

        std::stringstream time_stream;
        time_stream << "Time: " << g_window_data.time;

        update_user_inputs(g_game);

        game_update(g_game, g_window_data.delta_time);

        BeginDrawing(); 
            ClearBackground(SKYBLUE);

            if(g_settings.display_debug)
            {
                DrawFPS(8, 8);
                DrawText(time_stream.str().c_str(), 8, 32, 20, LIGHTGRAY);
            }

            game_draw(g_game, g_window_data.delta_time);
        EndDrawing();
    }

    CloseWindow();

    return 0;
}