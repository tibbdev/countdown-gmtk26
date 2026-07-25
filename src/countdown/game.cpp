#include "game.h"
#include <stddef.h>

#include "player.h"
#include "gazumpas.h"
#include "welcome.h"
#include "settings.h"
#include "common.h"
#include "countdown.h"

#include "raylib.h"

void back_button_draw(Vector2 position, bool is_hovered)
{
    DrawRectangleV(position, {BACK_BUTTON_WIDTH, BACK_BUTTON_HEIGHT}, ORANGE);
    DrawText("BAcK", position.x + (BACK_BUTTON_WIDTH >> 1) - (MeasureText("BAcK", BACK_BUTTON_TEXT_SIZE) >> 1), position.y + (BACK_BUTTON_HEIGHT >> 1) - (BACK_BUTTON_TEXT_SIZE >> 1), BACK_BUTTON_TEXT_SIZE, is_hovered ? YELLOW : BLACK);
}

void cursor_draw(Vector2 position)
{
    DrawCircleV(position, CURSOR_SIZE, GOLD);
}

void game_init(GameData &game)
{
    // Do nothing...
}
void game_update(GameData &game, float delta_time)
{
    switch(game.state)
    {
        case GameState::Welcome:
            welcome_update(delta_time, game);
            break;
        case GameState::Settings:
            settings_update(game, delta_time);
            break;
        case GameState::Playing:
        case GameState::Paused:
        case GameState::GameOver:
            countdown_update(game, delta_time);
            break;
        // case GameState::HighScores:
        //     break;
            
        default:
            break;
    }
}
void game_draw(GameData &game, float delta_time)
{
    switch(game.state)
    {
        case GameState::Welcome:
            welcome_draw(game);
            break;
        case GameState::Settings:
            settings_draw(game);
            break;
        case GameState::Playing:
            countdown_draw(game, delta_time);
            DrawText("Playing", (g_window_data.width  >> 1) - (MeasureText("Playing", 48)  >> 1), (g_window_data.height  >> 2) - (48 >> 1), 48, MAGENTA);
            break;
        case GameState::Paused:
            countdown_draw(game, delta_time);
            settings_draw(game);
        case GameState::GameOver:
            DrawText("GamE_OVeR", (g_window_data.width  >> 1) - (MeasureText("GamE_OVeR", 160)  >> 1), (g_window_data.height  >> 2) - (160 >> 1), 160, RAYWHITE);
            break;
        // case GameState::HighScores:
        //     break;
            
        default:
            break;
    }
}