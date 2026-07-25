#include "welcome.h"
#include "common.h"

constexpr int WELCOME_TEXT_SIZE = 96;
constexpr int HOVERED_TEXT_SIZE = 64;
constexpr int BUTTON_TEXT_SIZE  = 48;

constexpr int BUTTON_GAP = 24;

constexpr int BUTTON_RECT_WIDTH = 360;

// constexpr int BUTTON_RECT_HEIGHT = BUTTON_TEXT_SIZE + 16;

void welcome_button_draw(const char *text, int y, bool is_hovered)
{
    DrawRectangle((g_window_data.width >> 1) - (BUTTON_RECT_WIDTH >> 1), y - 8, BUTTON_RECT_WIDTH, BUTTON_TEXT_SIZE + 16, is_hovered ? LIME : DARKGREEN);
    
    if(is_hovered)
    {
        DrawCircle((g_window_data.width >> 1) - (BUTTON_RECT_WIDTH >> 1) - BUTTON_GAP, y + (BUTTON_TEXT_SIZE >> 1), 16, LIME);
        DrawCircle((g_window_data.width >> 1) + (BUTTON_RECT_WIDTH >> 1) + BUTTON_GAP, y + (BUTTON_TEXT_SIZE >> 1), 16, LIME);
    }
    
    DrawText(text, (g_window_data.width  >> 1) - (MeasureText(text, BUTTON_TEXT_SIZE)  >> 1), y, BUTTON_TEXT_SIZE, is_hovered ? RAYWHITE : LIME);
}

void welcome_update(float delta_time, GameData &game)
{
    int start_y = (g_window_data.height >> 1) - (BUTTON_TEXT_SIZE >> 1);

    game.welcome_data.is_start_hovered = CheckCollisionCircleRec(game.inputs.mouse_position, CURSOR_SIZE, {(float)((g_window_data.width >> 1) - (BUTTON_RECT_WIDTH >> 1)), (float)(start_y - 8), (float)BUTTON_RECT_WIDTH, (float)(BUTTON_TEXT_SIZE + 16)});
    
    start_y += BUTTON_TEXT_SIZE + BUTTON_GAP;

    game.welcome_data.is_settings_hovered = CheckCollisionCircleRec(game.inputs.mouse_position, CURSOR_SIZE, {(float)((g_window_data.width >> 1) - (BUTTON_RECT_WIDTH >> 1)), (float)(start_y - 8), (float)BUTTON_RECT_WIDTH, (float)(BUTTON_TEXT_SIZE + 16)});

    if(game.inputs.mouse_left_pressed)
    {
        if(game.welcome_data.is_start_hovered)
        {
            game.state      = GameState::Playing;
            game.game_time  = 0;
            game.level      = 0;

            game.welcome_data.is_start_hovered = false;
        }
        else if(game.welcome_data.is_settings_hovered)
        {
            game.state = GameState::Settings;

            game.welcome_data.is_settings_hovered = false;
        }
    }
}

void welcome_draw(GameData &game)
{
    DrawText("COUnTdoWN", (g_window_data.width  >> 1) - (MeasureText("COUnTdoWN", WELCOME_TEXT_SIZE)  >> 1), (g_window_data.height  >> 2) - (WELCOME_TEXT_SIZE >> 1), WELCOME_TEXT_SIZE, LIME);

    int start_y = (g_window_data.height >> 1) - (BUTTON_TEXT_SIZE >> 1);
    
    welcome_button_draw("StaRt", start_y, game.welcome_data.is_start_hovered);
    
    start_y += BUTTON_TEXT_SIZE + BUTTON_GAP;

    welcome_button_draw("SeTTinGS", start_y, game.welcome_data.is_settings_hovered);

    cursor_draw(game.inputs.mouse_position);
}
