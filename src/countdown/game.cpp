#include "game.h"
#include <stddef.h>
#include <cstdlib>
#include <math.h>

#include <string>
#include <sstream>

#include "player.h"
#include "gazumpas.h"
#include "welcome.h"
#include "settings.h"
#include "common.h"
#include "countdown.h"

#include "raylib.h"

constexpr int GO_BGRECT_POSITION_X  = 64;
constexpr int GO_BGRECT_POSITION_Y  = 64;
constexpr int GO_BGRECT_SIZE_OFFSET = 128;
constexpr int GO_BGRECT_BORDER_SIZE = 4;

constexpr int GO_BACK_BUTTON_OFFSET_X = 128;
constexpr int GO_BACK_BUTTON_OFFSET_Y = 24;

constexpr uint16_t GW_N_SECTORS = 36;

void back_button_draw(Vector2 position, bool is_hovered)
{
    DrawRectangleV(position, {BACK_BUTTON_WIDTH, BACK_BUTTON_HEIGHT}, ORANGE);
    DrawText("BAcK", position.x + (BACK_BUTTON_WIDTH * 0.5f) - (MeasureText("BAcK", BACK_BUTTON_TEXT_SIZE) * 0.5f), position.y + (BACK_BUTTON_HEIGHT * 0.5f) - (BACK_BUTTON_TEXT_SIZE * 0.5f), BACK_BUTTON_TEXT_SIZE, is_hovered ? YELLOW : BLACK);
}

void cursor_draw(Vector2 position)
{
    DrawCircleV(position, CURSOR_SIZE, GOLD);
}

void game_init(GameData &game)
{
    game.levels_completed = 0;
    game.levels.clear();

    { // Level 1 is always the same
        LevelData new_level = {};

        new_level.player_start_position = { 0.2f * ASSUMED_WORLD_SIZE, 0.45f * ASSUMED_WORLD_SIZE };

        KligData klig = {};
        klig.position = { 0.6f * ASSUMED_WORLD_SIZE, 0.33f * ASSUMED_WORLD_SIZE };
        new_level.kligs.push_back(klig);

        ZoneData home_zone = {};
        home_zone.position = { 0.8f * ASSUMED_WORLD_SIZE, 0.78f * ASSUMED_WORLD_SIZE };
        new_level.zones.push_back(home_zone);

        game.levels.push_back(new_level);
    }

    // srand((uint32_t)((g_window_data.time - (uint32_t)g_window_data.time) * 1000);

    bool sectors[GW_N_SECTORS] = {};

    for (uint16_t level_num = 1; 20 > level_num; level_num++)
    {
        LevelData new_level = {};
        
        for (uint16_t sector_idx = 0; GW_N_SECTORS > sector_idx; sector_idx++)
        {
            sectors[sector_idx] = false;
        }

        uint16_t player_sector = rand() % GW_N_SECTORS;
        sectors[player_sector] = true;

        // add kligs

        uint16_t n_kligs = 1 + (level_num / 3);

        std::vector<KligTribes> tribes = {};

        tribes.clear();
        tribes.push_back((KligTribes)(rand() % (uint8_t)KligTribes::KLIG_TRIBE_CNT));

        for (uint16_t tribe_idx = 0; (n_kligs / 2) > tribe_idx; tribe_idx++)
        {
            tribes.push_back((KligTribes)(rand() % (uint8_t)KligTribes::KLIG_TRIBE_CNT));
        }

        new_level.kligs.clear();

        for (uint16_t klig_idx = 0; n_kligs > klig_idx; klig_idx++)
        {
            KligData new_klig = {};

            uint16_t tribe_idx = rand() % (uint8_t)tribes.size();

            new_klig.tribe = tribes[tribe_idx];

            if (0 == tribe_idx)
            {
                new_klig.color = RED;
            }
            else if (1 == tribe_idx)
            {
                new_klig.color = DARKBLUE;
            }
            else if (2 == tribe_idx)
            {
                new_klig.color = DARKPURPLE;
            }
            else if (3 == tribe_idx)
            {
                new_klig.color = BROWN;
            }
            else if (4 == tribe_idx)
            {
                new_klig.color = BLACK;
            }
            else
            {
                new_klig.color = VIOLET;
            }

            uint16_t nklig_sector = rand() % GW_N_SECTORS;

            while (sectors[nklig_sector])
            {
                nklig_sector = rand() % GW_N_SECTORS;
            }

            uint16_t col = nklig_sector % 6;
            uint16_t row = nklig_sector / 6;


            new_klig.start_position.x = col * (ASSUMED_WORLD_SIZE / 6) + (ASSUMED_WORLD_SIZE / 12);
            new_klig.start_position.y = row * (ASSUMED_WORLD_SIZE / 6) + (ASSUMED_WORLD_SIZE / 12);

            new_klig.position = new_klig.start_position;

            new_level.kligs.push_back(new_klig);
        }

        // add zones
        uint16_t n_zones = tribes.size();

        new_level.zones.clear();

        for (uint16_t zone_idx = 0; n_zones > zone_idx; zone_idx++)
        {
            ZoneData new_zone = {};

            new_zone.type = ZoneData::ZoneType::Home;

            new_zone.tribe = tribes[zone_idx];

            if (0 == zone_idx)
            {
                new_zone.color = RED;
            }
            else if (1 == zone_idx)
            {
                new_zone.color = DARKBLUE;
            }
            else if (2 == zone_idx)
            {
                new_zone.color = DARKPURPLE;
            }
            else if (3 == zone_idx)
            {
                new_zone.color = BROWN;
            }
            else if (4 == zone_idx)
            {
                new_zone.color = BLACK;
            }
            else
            {
                new_zone.color = VIOLET;
            }

            uint16_t nzone_sector = rand() % GW_N_SECTORS;

            while (sectors[nzone_sector])
            {
                nzone_sector = rand() % GW_N_SECTORS;
            }

            uint16_t col = nzone_sector % 6;
            uint16_t row = nzone_sector / 6;

            new_zone.position.x = col * (ASSUMED_WORLD_SIZE / 6) + (ASSUMED_WORLD_SIZE / 12);
            new_zone.position.y = row * (ASSUMED_WORLD_SIZE / 6) + (ASSUMED_WORLD_SIZE / 12);

            new_level.zones.push_back(new_zone);
        }

        game.levels.push_back(new_level);
    }

    game.player.position = game.levels[game.level].player_start_position;
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
            countdown_update(game, delta_time);
            break;
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
            break;
        case GameState::Paused:
            {
                countdown_draw(game, delta_time);
                settings_draw(game);
            }
            break;
        case GameState::GameOver:
            {
                Vector2 rect_size = {(float)g_window_data.width - GO_BGRECT_SIZE_OFFSET, (float)g_window_data.height - GO_BGRECT_SIZE_OFFSET};
                Vector2 rect_position = {GO_BGRECT_POSITION_X, GO_BGRECT_POSITION_Y};

                DrawRectangleV(rect_position, rect_size, DARKGRAY);
                DrawRectangleLines(rect_position.x, rect_position.y, rect_size.x, rect_size.y, LIGHTGRAY);

                DrawText("GamE_OVeR", (g_window_data.width  * 0.5f) - (MeasureText("GamE_OVeR", 160)  * 0.5f), (g_window_data.height  * 0.33f) - (160 * 0.5f), 160, RAYWHITE);

                std::stringstream levels_completed_sstr;
                levels_completed_sstr << "LEveLs_CoMPletEd " << game.levels_completed;
                DrawText(levels_completed_sstr.str().c_str(), (g_window_data.width * 0.5f) - (MeasureText(levels_completed_sstr.str().c_str(), 120) * 0.5f), (g_window_data.height * 0.5f) + 50, 120, LIGHTGRAY);

                std::stringstream game_time_sstr;
                game_time_sstr << "tiMe " << roundf(game.gameover_at * 100) * 0.01f;
                DrawText(game_time_sstr.str().c_str(), (g_window_data.width * 0.5f) - (MeasureText(levels_completed_sstr.str().c_str(), 120) * 0.5f), (g_window_data.height * 0.5f) + 250, 120, LIGHTGRAY);

                cursor_draw(game.inputs.mouse_position);
            } 
            break;
        // case GameState::HighScores:
        //     break;
            
        default:
            break;
    }
}