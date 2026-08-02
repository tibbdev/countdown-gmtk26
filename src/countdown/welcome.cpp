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
    DrawRectangle((g_window_data.width * 0.5f) - (BUTTON_RECT_WIDTH * 0.5f), y - 8, BUTTON_RECT_WIDTH, BUTTON_TEXT_SIZE + 16, is_hovered ? LIME : DARKGREEN);
    
    if(is_hovered)
    {
        DrawCircle((g_window_data.width * 0.5f) - (BUTTON_RECT_WIDTH * 0.5f) - BUTTON_GAP, y + (BUTTON_TEXT_SIZE * 0.5f), 16, LIME);
        DrawCircle((g_window_data.width * 0.5f) + (BUTTON_RECT_WIDTH * 0.5f) + BUTTON_GAP, y + (BUTTON_TEXT_SIZE * 0.5f), 16, LIME);
    }
    
    DrawText(text, (g_window_data.width  * 0.5f) - (MeasureText(text, BUTTON_TEXT_SIZE)  * 0.5f), y, BUTTON_TEXT_SIZE, is_hovered ? RAYWHITE : LIME);
}

void welcome_update(float delta_time, GameData &game)
{
    int start_y = (g_window_data.height * 0.5f) - (BUTTON_TEXT_SIZE * 0.5f);

    game.welcome_data.is_start_hovered = CheckCollisionCircleRec(game.inputs.mouse_position, CURSOR_SIZE, {(float)((g_window_data.width * 0.5f) - (BUTTON_RECT_WIDTH * 0.5f)), (float)(start_y - 8), (float)BUTTON_RECT_WIDTH, (float)(BUTTON_TEXT_SIZE + 16)});
    
    start_y += BUTTON_TEXT_SIZE + BUTTON_GAP;

    game.welcome_data.is_settings_hovered = CheckCollisionCircleRec(game.inputs.mouse_position, CURSOR_SIZE, {(float)((g_window_data.width * 0.5f) - (BUTTON_RECT_WIDTH * 0.5f)), (float)(start_y - 8), (float)BUTTON_RECT_WIDTH, (float)(BUTTON_TEXT_SIZE + 16)});

    if(game.inputs.mouse_left_pressed)
    {
        if(game.welcome_data.is_start_hovered)
        {
            game.state            = GameState::Playing;
            game.game_time        = 0;
            game.level            = 0;
            game.levels_completed = 0;

            game.safe_at     = 0.0f;
            game.gameover_at = 0.0f;

            for (LevelData& level : game.levels)
            {
                level.player_failed  = false;
                level.player_success = false;

                for (KligData& klig : level.kligs)
                {
                    klig.state         = KligState::CountingUp;
                    klig.count         = 0;
                    klig.last_count_at = 0.0f;
                }
            }

            game.welcome_data.is_start_hovered = false;
        }
        /*else if(game.welcome_data.is_settings_hovered)
        {
            game.state = GameState::Settings;

            game.welcome_data.is_settings_hovered = false;
        }*/
    }
}

void welcome_draw(GameData &game)
{
    DrawText("COUnTdoWN", (g_window_data.width  * 0.5f) - (MeasureText("COUnTdoWN", WELCOME_TEXT_SIZE)  * 0.5f), (g_window_data.height  * 0.33f) - (WELCOME_TEXT_SIZE * 0.5f), WELCOME_TEXT_SIZE, LIME);

    int start_y = (g_window_data.height * 0.5f) - (BUTTON_TEXT_SIZE * 0.5f);
    
    welcome_button_draw("StaRt", start_y, game.welcome_data.is_start_hovered);
    
    /*start_y += BUTTON_TEXT_SIZE + BUTTON_GAP;

    welcome_button_draw("SeTTinGS", start_y, game.welcome_data.is_settings_hovered);*/

    DrawText("prESs_tHE_EScApe_KEy_tO_QUIt", (g_window_data.width * 0.5f) - (MeasureText("prESs_tHE_EScApe_KEy_tO_QUIt", WELCOME_TEXT_SIZE) * 0.5f), g_window_data.height - (g_window_data.height * 0.33f) - (WELCOME_TEXT_SIZE * 0.5f), WELCOME_TEXT_SIZE, RAYWHITE);

    cursor_draw(game.inputs.mouse_position);
}
