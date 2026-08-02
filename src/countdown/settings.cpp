#include "settings.h"
#include "common.h"
#include "raylib.h"

GameSettings g_settings;

constexpr int SETTINGS_TEXT_SIZE = 96;

constexpr int BGRECT_POSITION_X  = 64;
constexpr int BGRECT_POSITION_Y  = 64;
constexpr int BGRECT_SIZE_OFFSET = 128;

constexpr int BACK_BUTTON_OFFSET_X = 128;
constexpr int BACK_BUTTON_OFFSET_Y = 24;

void settings_update(GameData &game, float delta_time)
{
    g_settings.is_back_hovered = CheckCollisionCircleRec(game.inputs.mouse_position, CURSOR_SIZE, {(float)(g_window_data.width - BACK_BUTTON_OFFSET_X), (float)(BGRECT_POSITION_Y + BACK_BUTTON_OFFSET_Y), (float)BACK_BUTTON_WIDTH, (float)BACK_BUTTON_HEIGHT});

    if(game.inputs.mouse_left_pressed && g_settings.is_back_hovered)
    {
        if (GameState::Paused == game.state)
        {
            game.state = GameState::Playing;
        }
        else
        {
            game.state = GameState::Welcome;
        }

        g_settings.is_back_hovered = false;
    }
}

void settings_draw(GameData &game)
{
    Vector2 rect_size = {(float)g_window_data.width - BGRECT_SIZE_OFFSET, (float)g_window_data.height - BGRECT_SIZE_OFFSET};
    Vector2 rect_position = {BGRECT_POSITION_X, BGRECT_POSITION_Y};

    DrawRectangleV(rect_position, rect_size, DARKPURPLE);

    DrawText("SeTTinGS", (g_window_data.width  * 0.5f) - (MeasureText("SeTTinGS", SETTINGS_TEXT_SIZE)  * 0.5f), (g_window_data.height  * 0.33f) - (SETTINGS_TEXT_SIZE * 0.5f), SETTINGS_TEXT_SIZE, MAGENTA);

    back_button_draw({rect_position.x + rect_size.x - BACK_BUTTON_OFFSET_X, rect_position.y + BACK_BUTTON_OFFSET_Y}, g_settings.is_back_hovered);
    
    cursor_draw(game.inputs.mouse_position);
}