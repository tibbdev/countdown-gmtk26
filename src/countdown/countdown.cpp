#include "countdown.h"
#include "common.h"
#include "settings.h"

#include <string>
#include <sstream>

constexpr float COUNTDOWN_DRAW_WORLD_WIDTH_RATIO = 0.7f;

constexpr uint32_t ASSUMED_WORLD_SIZE         = 2048;
constexpr uint32_t COUNTDOWN_DRAW_WRLD_BORDER = 64;
constexpr uint32_t PLAYER_SIZE                = 32;
constexpr uint32_t GAZUMPA_SIZE               = 24;
constexpr uint32_t KLIG_SIZE                  = 10;

void countdown_update(GameData &game, float delta_time)
{
    switch (game.state)
    {
        case GameState::Playing:
            {
                if (game.inputs.kb_paused_pressed)
                {
                    game.state = GameState::Paused;
                }
                else
                {
                    game.game_time += delta_time;

                    // world space is -1 to 1
                    Rectangle world = {};
                    world.x         = -1;
                    world.y         = -1;
                    world.width     = 2;
                    world.height    = 2;

                    game.player.velocity = {0.0f, 0.0f};

                    // Vertical Movement
                    if (game.inputs.kb_move_up_pressed)
                    {
                        game.player.velocity.y -= game.player.speed;
                    }                    
                    if (game.inputs.kb_move_down_pressed)
                    {
                        game.player.velocity.y += game.player.speed;
                    }
                    
                    // Horizontal Movement
                    if (game.inputs.kb_move_left_pressed)
                    {
                        game.player.velocity.x -= game.player.speed;
                    }                    
                    if (game.inputs.kb_move_right_pressed)
                    {
                        game.player.velocity.x += game.player.speed;
                    }

                    // Attempt the move
                    game.player.position.x += delta_time * game.player.velocity.x;
                    game.player.position.y += delta_time * game.player.velocity.y;

                    float assumed_player_size = (float)PLAYER_SIZE / ASSUMED_WORLD_SIZE;
                    float assumed_player_half = assumed_player_size * 0.5f;

                    Rectangle player = {};
                    player.x         = game.player.position.x - assumed_player_half;
                    player.y         = game.player.position.y - assumed_player_half;
                    player.width     = assumed_player_size;
                    player.height    = assumed_player_size;

                    // Check the player is still within the world
                    // if (!CheckCollisionRecs(world, player))
                    {
                        if (-0.99f > (game.player.position.x - assumed_player_half))
                        {
                            game.player.position.x = -0.99f + assumed_player_half;
                        }
                        else if (0.99f < (game.player.position.x + assumed_player_half))
                        {
                            game.player.position.x = 0.99f - assumed_player_half;
                        }

                        if (-0.99f > (game.player.position.y - assumed_player_half))
                        {
                            game.player.position.y = -0.99f + assumed_player_half;
                        }
                        else if (0.99f < (game.player.position.y + assumed_player_half))
                        {
                            game.player.position.y = 0.99f - assumed_player_half;
                        }
                    }

                    bool player_failed = false;

                    for (KligData& klig : game.levels[game.level].kligs)
                    {
                        klig.is_grabbable = CheckCollisionCircleRec(klig.position, (float)(KLIG_SIZE + 4) / ASSUMED_WORLD_SIZE, player);
                        klig.is_homable = false;

                        for (ZoneData& zone : game.levels[game.level].zones)
                        {
                            if ((ZoneData::ZoneType::Home == zone.type) && (zone.tribe == klig.tribe))
                            {
                                float assumed_zone_size_x = (float)zone.size.x / ASSUMED_WORLD_SIZE;
                                float assumed_zone_half_x = assumed_zone_size_x * 0.5f;

                                float assumed_zone_size_y = (float)zone.size.y / ASSUMED_WORLD_SIZE;
                                float assumed_zone_half_y = assumed_zone_size_y * 0.5f;

                                Rectangle zoner = {};
                                zoner.x = zone.position.x - assumed_zone_half_x;
                                zoner.y = zone.position.y - assumed_zone_half_y;
                                zoner.width = assumed_zone_size_x;
                                zoner.height = assumed_zone_size_y;

                                klig.is_homable = CheckCollisionCircleRec(klig.position, (float)(KLIG_SIZE + 4) / ASSUMED_WORLD_SIZE, zoner);
                            }
                        }

                        klig.is_grabbed = false;
                        klig.is_home    = false;

                        switch (klig.state)
                        {
                            case KligState::CountingUp:
                                if (game.game_time > (klig.last_count_at + game.levels[game.level].klig_count_rate))
                                {
                                    klig.last_count_at = game.game_time;
                                    klig.count++;


                                    if (30 < klig.count)
                                    {
                                        //player_failed = true;

                                        klig.count = 30;
                                        klig.state = KligState::Overloaded; // Game over condition...
                                    }
                                }
                                if (klig.is_grabbable && (game.inputs.kb_grab_pressed || game.inputs.gpad_grab_pressed))
                                {
                                    klig.state = KligState::Grabbed;
                                }
                                break;
                            case KligState::Grabbed:
                                klig.is_grabbed = true;

                                klig.position = game.player.position;
                                if (game.inputs.kb_grab_pressed || game.inputs.gpad_grab_pressed)
                                {
                                    if (klig.is_homable)
                                    {
                                        klig.state = KligState::CountingDown;
                                    }
                                    else
                                    {
                                        klig.state = KligState::CountingUp;
                                    }

                                }
                                break;
                            case KligState::CountingDown:
                                klig.is_home = true;

                                if (game.game_time > (klig.last_count_at + game.levels[game.level].klig_count_rate))
                                {
                                    klig.last_count_at = game.game_time;
                                    klig.count--;

                                    if (0 >= klig.count)
                                    {
                                        klig.count = 0;
                                        klig.state = KligState::Safe;
                                    }
                                }

                                if (klig.is_grabbable && (game.inputs.kb_grab_pressed || game.inputs.gpad_grab_pressed))
                                {
                                    klig.state = KligState::Grabbed;
                                }
                                break;
                            case KligState::Overloaded:
                                //player_failed = true;
                                break;

                            default:
                                break;
                        }
                    }

                    if (player_failed)
                    {
                        game.state = GameState::GameOver;
                    }
                }
            }
            break;
        case GameState::Paused:
            {
                settings_update(game, delta_time);
            }
            break;
        case GameState::GameOver:
            {
                // TODO : Fill in with useful code
            }
            break;

        default:
            break;
    }
}

void countdown_draw(GameData &game, float delta_time)
{
    float world_draw_space = COUNTDOWN_DRAW_WORLD_WIDTH_RATIO * g_window_data.width;
    world_draw_space = (world_draw_space - (2*COUNTDOWN_DRAW_WRLD_BORDER)) > g_window_data.height - (2*COUNTDOWN_DRAW_WRLD_BORDER) ? g_window_data.height - (2*COUNTDOWN_DRAW_WRLD_BORDER) : (world_draw_space - (2*COUNTDOWN_DRAW_WRLD_BORDER));

    Vector2 world_draw_position = {((COUNTDOWN_DRAW_WORLD_WIDTH_RATIO * g_window_data.width) - world_draw_space) / 2 , (g_window_data.height - world_draw_space) / 2};

    DrawRectangleV(world_draw_position, {world_draw_space, world_draw_space}, DARKGRAY);

    float draw_scale = world_draw_space / ASSUMED_WORLD_SIZE;

    for (ZoneData& zone : game.levels[game.level].zones)
    {
        DrawRectangle( draw_scale * (zone.position.x * ASSUMED_WORLD_SIZE + ASSUMED_WORLD_SIZE) * 0.5f, draw_scale * (zone.position.y * ASSUMED_WORLD_SIZE + ASSUMED_WORLD_SIZE) * 0.5f, draw_scale * zone.size.x, draw_scale * zone.size.y, DARKPURPLE);
    }

    // player size
    float assumed_player_size = (float)PLAYER_SIZE;
    float assumed_player_half = assumed_player_size * 0.5f;

    float assumed_player_x = (ASSUMED_WORLD_SIZE * game.player.position.x + ASSUMED_WORLD_SIZE) * 0.5f;
    float assumed_player_y = (ASSUMED_WORLD_SIZE * game.player.position.y + ASSUMED_WORLD_SIZE) * 0.5f;

    Rectangle player = {};
    player.x      = world_draw_position.x + draw_scale * assumed_player_x; 
    player.y      = world_draw_position.y + draw_scale * assumed_player_y; 
    player.width  = draw_scale * assumed_player_size;
    player.height = draw_scale * assumed_player_size;

    DrawRectangleRec(player, RAYWHITE);

    for (KligData &klig : game.levels[game.level].kligs)
    {
        float klig_x = draw_scale * (klig.position.x * ASSUMED_WORLD_SIZE + ASSUMED_WORLD_SIZE) * 0.5f;
        float klig_y = draw_scale * (klig.position.y * ASSUMED_WORLD_SIZE + ASSUMED_WORLD_SIZE) * 0.5f;

        DrawCircleV({ klig_x, klig_y }, draw_scale * KLIG_SIZE, klig.is_grabbed ? PINK : PURPLE);

        if (klig.is_grabbable && !klig.is_grabbed)
        {
            DrawCircleLinesV({ klig_x, klig_y }, draw_scale * KLIG_SIZE, RAYWHITE);
        }
        else if (klig.is_homable && !klig.is_home)
        {
            DrawCircleLinesV({ klig_x, klig_y }, draw_scale * KLIG_SIZE, YELLOW);
        }

        if (!klig.is_grabbed)
        {
            std::stringstream klig_cnt_sstr;
            klig_cnt_sstr << klig.count;
            DrawText(klig_cnt_sstr.str().c_str(), 1 + klig_x - (0.5f * MeasureText(klig_cnt_sstr.str().c_str(), 0.96f * KLIG_SIZE)), 1 + klig_y - (KLIG_SIZE * 0.48f), 0.96f * KLIG_SIZE, DARKPURPLE);
        }
    }

    DrawRectangleV({ COUNTDOWN_DRAW_WORLD_WIDTH_RATIO * g_window_data.width, 0.0f }, { (1.0f - COUNTDOWN_DRAW_WORLD_WIDTH_RATIO)*g_window_data.width, (float)g_window_data.height }, DARKBLUE);
    
    constexpr uint32_t DBG_FONT_SIZE = 14;

    std::stringstream draw_scale_sstr;
    draw_scale_sstr << "DRAW_SCALE := " << draw_scale;
    DrawText(draw_scale_sstr.str().c_str(), COUNTDOWN_DRAW_WORLD_WIDTH_RATIO * g_window_data.width + 16, 16, DBG_FONT_SIZE, WHITE);
    
    std::stringstream player_position_sstr;
    player_position_sstr << "PLAYER_POSITION := {" << (uint32_t)player.x << ", " << (uint32_t)player.y << "}";
    DrawText(player_position_sstr.str().c_str(), COUNTDOWN_DRAW_WORLD_WIDTH_RATIO * g_window_data.width + 16, 32, DBG_FONT_SIZE, WHITE);
    
    std::stringstream game_player_position_sstr;
    game_player_position_sstr << "PLAYER_POSITION := {" << game.player.position.x << ", " << game.player.position.y << "}";
    DrawText(game_player_position_sstr.str().c_str(), COUNTDOWN_DRAW_WORLD_WIDTH_RATIO * g_window_data.width + 16, 48, DBG_FONT_SIZE, WHITE);

    std::stringstream draw_size_sstr;
    draw_size_sstr << "PLAYER_SIZE := " << (uint32_t)assumed_player_size;
    DrawText(draw_size_sstr.str().c_str(), COUNTDOWN_DRAW_WORLD_WIDTH_RATIO * g_window_data.width + 16, 64, DBG_FONT_SIZE, WHITE);

    std::stringstream klig_size_sstr;
    klig_size_sstr << "KLIG_SIZE := " << (uint32_t) KLIG_SIZE;
    DrawText(klig_size_sstr.str().c_str(), COUNTDOWN_DRAW_WORLD_WIDTH_RATIO * g_window_data.width + 16, 80, DBG_FONT_SIZE, WHITE);

    if (0 != game.levels[game.level].kligs.size())
    {
        std::stringstream klig_count_at_sstr;
        klig_count_at_sstr << "KLIG_COUNT_AT := " << game.levels[game.level].kligs[0].last_count_at;
        DrawText(klig_count_at_sstr.str().c_str(), COUNTDOWN_DRAW_WORLD_WIDTH_RATIO * g_window_data.width + 16, 128, DBG_FONT_SIZE, WHITE);

        std::stringstream klig_count_sstr;
        klig_count_sstr << "KLIG_COUNT := " << game.levels[game.level].kligs[0].count;
        DrawText(klig_count_sstr.str().c_str(), COUNTDOWN_DRAW_WORLD_WIDTH_RATIO * g_window_data.width + 16, 144, DBG_FONT_SIZE, WHITE);

        DrawText("Grabable", COUNTDOWN_DRAW_WORLD_WIDTH_RATIO * g_window_data.width + 16, 160, DBG_FONT_SIZE, game.levels[game.level].kligs[0].is_grabbable ? LIME : RED);
        DrawText("Homable", COUNTDOWN_DRAW_WORLD_WIDTH_RATIO * g_window_data.width + 16, 176, DBG_FONT_SIZE, game.levels[game.level].kligs[0].is_homable ? LIME : RED);

        std::stringstream klig_position_sstr;
        klig_position_sstr << "KLIG_POSITION := {" << game.levels[game.level].kligs[0].position.x << ", " << game.levels[game.level].kligs[0].position.y << "}";
        DrawText(klig_position_sstr.str().c_str(), COUNTDOWN_DRAW_WORLD_WIDTH_RATIO * g_window_data.width + 16, 192, DBG_FONT_SIZE, WHITE);
    }
}
