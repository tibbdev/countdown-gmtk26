#include "countdown.h"
#include "common.h"
#include "settings.h"

#include <math.h>

#include <string>
#include <sstream>

constexpr float COUNTDOWN_DRAW_WORLD_WIDTH_RATIO = 0.7f;

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
                    world.x         = 0;
                    world.y         = 0;
                    world.width     = ASSUMED_WORLD_SIZE;
                    world.height    = ASSUMED_WORLD_SIZE;

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

                    float assumed_player_size = (float)PLAYER_SIZE;
                    float assumed_player_half = assumed_player_size * 0.5f;

                    Rectangle playerrec = {};
                    playerrec.x         = game.player.position.x - assumed_player_half;
                    playerrec.y         = game.player.position.y - assumed_player_half;
                    playerrec.width     = assumed_player_size;
                    playerrec.height    = assumed_player_size;

                    // Check the player is still within the world
                    // if (!CheckCollisionRecs(world, player))
                    {
                        if ((4 > (playerrec.x - assumed_player_half)) && (0 > game.player.velocity.x))
                        {
                            game.player.position.x = 4 + assumed_player_half;
                        }
                        else if (((ASSUMED_WORLD_SIZE - 4) < (playerrec.x + assumed_player_half)) && (0 < game.player.velocity.x))
                        {
                            game.player.position.x = (ASSUMED_WORLD_SIZE - 4) - assumed_player_half;
                        }

                        if ((4 > (playerrec.y - assumed_player_half)) && (0 > game.player.velocity.y))
                        {
                            game.player.position.y = 4 + assumed_player_half;
                        }
                        else if (((ASSUMED_WORLD_SIZE - 4) < (playerrec.y + assumed_player_half)) && (0 < game.player.velocity.y))
                        {
                            game.player.position.y = (ASSUMED_WORLD_SIZE - 4) - assumed_player_half;
                        }
                    }

                    game.levels[game.level].player_failed  = false;
                    game.levels[game.level].player_success = true;

                    for (KligData& klig : game.levels[game.level].kligs)
                    {
                        klig.is_grabbable = CheckCollisionCircleRec(klig.position, (float)(KLIG_SIZE + 4), playerrec);
                        klig.is_homable = false;

                        for (ZoneData& zone : game.levels[game.level].zones)
                        {
                            if ((ZoneData::ZoneType::Home == zone.type) && (zone.tribe == klig.tribe))
                            {
                                float assumed_zone_size_x = (float)zone.size.x;
                                float assumed_zone_half_x = assumed_zone_size_x * 0.5f;

                                float assumed_zone_size_y = (float)zone.size.y;
                                float assumed_zone_half_y = assumed_zone_size_y * 0.5f;

                                Rectangle zoner = {};
                                zoner.x = zone.position.x - assumed_zone_half_x;
                                zoner.y = zone.position.y - assumed_zone_half_y;
                                zoner.width = assumed_zone_size_x;
                                zoner.height = assumed_zone_size_y;

                                klig.is_homable |= CheckCollisionCircleRec(klig.position, (float)(KLIG_SIZE + 4), zoner);
                            }
                        }

                        klig.is_grabbed = false;
                        klig.is_home    = false;

                        switch (klig.state)
                        {
                            case KligState::CountingUp:
                                game.levels[game.level].player_success = false;

                                if (game.game_time > (klig.last_count_at + game.levels[game.level].klig_count_rate))
                                {
                                    klig.last_count_at = game.game_time;
                                    klig.count++;


                                    if (30 < klig.count)
                                    {
                                        // player_failed = true;

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

                                game.levels[game.level].player_success = false;

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

                                game.levels[game.level].player_success = false;

                                if (game.game_time > (klig.last_count_at + (game.levels[game.level].klig_count_rate * 0.6f)))
                                {
                                    klig.last_count_at = game.game_time;
                                    klig.count--;

                                    if (0 >= klig.count)
                                    {
                                        klig.count = 0;
                                        klig.state = KligState::Safe;

                                        game.safe_at = game.game_time;
                                    }
                                }

                                if (klig.is_grabbable && (game.inputs.kb_grab_pressed || game.inputs.gpad_grab_pressed))
                                {
                                    klig.state = KligState::Grabbed;
                                }
                                break;
                            case KligState::Overloaded:
                                game.levels[game.level].player_failed = true;
                                game.levels[game.level].player_success = false;
                                game.gameover_at = game.game_time;
                                break;

                            default:
                                break;
                        }
                    }

                    if (game.levels[game.level].player_success)
                    {
                        if (game.game_time > (game.safe_at + 1.6f))
                        {
                            game.player.position                   = game.levels[game.level].player_start_position;
                            game.levels[game.level].player_success = false;
                            game.level++;
                            game.levels_completed++;

                            if (game.levels.size() <= game.level)
                            {
                                game.level = 1;
                            }
                        }
                    }
                    else if (game.levels[game.level].player_failed)
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
                game.game_time += delta_time;
                if (game.game_time > (game.gameover_at + 10.0f))
                {
                    game.state = GameState::Welcome;
                }
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

    DrawRectangleV({ COUNTDOWN_DRAW_WORLD_WIDTH_RATIO * g_window_data.width, 0.0f }, { (1.0f - COUNTDOWN_DRAW_WORLD_WIDTH_RATIO) * g_window_data.width, (float)g_window_data.height }, DARKBLUE);

    DrawRectangleV(world_draw_position, {world_draw_space, world_draw_space}, DARKGRAY);

    float draw_scale = world_draw_space / ASSUMED_WORLD_SIZE;

    for (ZoneData& zone : game.levels[game.level].zones)
    {
        if (ZoneData::ZoneType::Home == zone.type)
        {
            DrawRectangle(world_draw_position.x + draw_scale * zone.position.x, world_draw_position.y + draw_scale * zone.position.y, draw_scale * zone.size.x, draw_scale * zone.size.y, zone.color);
        }
    }

    // player size
    float assumed_player_size = (float)PLAYER_SIZE;
    float assumed_player_half = assumed_player_size * 0.5f;

    Rectangle player = {};
    player.x      = world_draw_position.x + draw_scale * game.player.position.x - draw_scale * (PLAYER_SIZE * 0.5f);
    player.y      = world_draw_position.y + draw_scale * game.player.position.y - draw_scale * (PLAYER_SIZE * 0.5f);
    player.width  = draw_scale * assumed_player_size;
    player.height = draw_scale * assumed_player_size;

    DrawRectangleRec(player, RAYWHITE);

    uint16_t unsafe_kligs = 0;

    for (KligData &klig : game.levels[game.level].kligs)
    {
        if (KligState::Safe != klig.state)
        {
            float klig_x = world_draw_position.x + draw_scale * klig.position.x;
            float klig_y = world_draw_position.y + draw_scale * klig.position.y;
            // 4 x 5 @ 168, 0 in atlas, 3 sprites (normal, grab, home)

            unsafe_kligs++;

            Color tribe_colour = WHITE;

            for (auto& zone : game.levels[game.level].zones)
            {
                if (zone.tribe == klig.tribe)
                {
                    tribe_colour = zone.color;
                    break;
                }
            }

            Color klig_colour_to_draw = klig.is_grabbed ? (klig.is_homable ? ORANGE : PINK) : (klig.is_grabbable ? LIME : tribe_colour);

            DrawCircleV({ klig_x, klig_y }, draw_scale * KLIG_SIZE, klig_colour_to_draw);

            if (klig.is_grabbable && !klig.is_grabbed)
            {
                if (game.level < 5)
                {
                    DrawText("GrAB NOw!", COUNTDOWN_DRAW_WORLD_WIDTH_RATIO * g_window_data.width + 16, (g_window_data.height * 0.5f) - 248, 24, LIME);
                }

                DrawCircleLinesV({ klig_x, klig_y }, (draw_scale * KLIG_SIZE),     klig.color);
                DrawCircleLinesV({ klig_x, klig_y }, (draw_scale * KLIG_SIZE) + 1, klig.color);

                DrawCircleLinesV({ klig_x, klig_y }, 2 + draw_scale * KLIG_SIZE, DARKGREEN);
            }
            else if (klig.is_homable && !klig.is_home)
            {
                if (game.level < 5)
                {
                    DrawText("REleAsE nOW!", COUNTDOWN_DRAW_WORLD_WIDTH_RATIO * g_window_data.width + 16, (g_window_data.height * 0.5f) - 220, 24, ORANGE);
                }

                DrawCircleLinesV({ klig_x, klig_y }, (draw_scale * KLIG_SIZE), klig.color);
                DrawCircleLinesV({ klig_x, klig_y }, (draw_scale * KLIG_SIZE) + 1, klig.color);

                DrawCircleLinesV({ klig_x, klig_y }, 2 + draw_scale * KLIG_SIZE, DARKBROWN);
            }

            if (!klig.is_grabbed)
            {
                std::stringstream klig_cnt_sstr;
                klig_cnt_sstr << klig.count;
                DrawText(klig_cnt_sstr.str().c_str(), 1 + klig_x - (0.5f * MeasureText(klig_cnt_sstr.str().c_str(), 0.96f * KLIG_SIZE)), 1 + klig_y - (KLIG_SIZE * 0.48f), 0.96f * KLIG_SIZE, DARKPURPLE);
            }
        }
    }
    
    constexpr uint32_t FONT_SIZE = 160;

    std::stringstream gametime_sstr;

    if (game.levels[game.level].player_success)
    {
        gametime_sstr << roundf(game.safe_at * 100) * 0.01f;
    }
    else
    {
        gametime_sstr << roundf(game.game_time * 100) * 0.01f;
    }
    DrawText(gametime_sstr.str().c_str(), COUNTDOWN_DRAW_WORLD_WIDTH_RATIO * g_window_data.width + 16, 16, FONT_SIZE, YELLOW);

    std::stringstream level_sstr;
    level_sstr << "LeVEl " << game.level + 1;
    DrawText(level_sstr.str().c_str(), COUNTDOWN_DRAW_WORLD_WIDTH_RATIO * g_window_data.width + 16, 16 + (uint32_t)(FONT_SIZE * 1.1f), (uint32_t)(0.8f * FONT_SIZE), BLUE);

    std::stringstream unsafe_cnt_sstr;
    unsafe_cnt_sstr << "uNSafE_kLIgS " << unsafe_kligs;
    DrawText(unsafe_cnt_sstr.str().c_str(), COUNTDOWN_DRAW_WORLD_WIDTH_RATIO * g_window_data.width + 16, 16 + (uint32_t)(FONT_SIZE * 2.0f), 36, MAGENTA);

    DrawText("PUt aLl DoTS in TheIr HOmEs beFoRE", COUNTDOWN_DRAW_WORLD_WIDTH_RATIO * g_window_data.width + 16, (g_window_data.height * 0.5f) - 64, 24, RAYWHITE);
    DrawText("tHeY ReACh 30.", COUNTDOWN_DRAW_WORLD_WIDTH_RATIO * g_window_data.width + 16, (g_window_data.height * 0.5f) - 34, 24, RAYWHITE);

    DrawText("WHeN GReEn  : PreSS G tO GraB", COUNTDOWN_DRAW_WORLD_WIDTH_RATIO * g_window_data.width + 16, (g_window_data.height * 0.5f), 24, RAYWHITE);
    DrawText("WHeN ORaNGe : PreSS G tO rELeaSE", COUNTDOWN_DRAW_WORLD_WIDTH_RATIO * g_window_data.width + 16, (g_window_data.height * 0.5f) + 28, 24, RAYWHITE);

    if (game.levels[game.level].player_success)
    {
        DrawText("lEVeL_cOMpLetE", 1 + (g_window_data.width * 0.5f) - (0.5f * MeasureText("lEVeL_cOMpLetE", 192)), 1 + (g_window_data.height * 0.5f) - (192 * 0.48f), 192, ORANGE);
    }
}
