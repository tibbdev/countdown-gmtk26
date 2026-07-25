#include "countdown.h"
#include "common.h"
#include "settings.h"

#include <string>
#include <sstream>

constexpr float COUNTDOWN_DRAW_WORLD_WIDTH_RATIO = 0.7f;

constexpr uint32_t ASSUMED_WORLD_SIZE         = 2000;
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

                    Vector2 current_pos = game.player.position;

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
                    if (!CheckCollisionRecs(world, player))
                    {
                        // if not, put them back where they were
                        game.player.position = current_pos;

                        if (-1.0f > (game.player.position.x - assumed_player_half))
                        {
                            game.player.position.x = -1.0f + assumed_player_half;
                        }
                        else if (1.0f < (game.player.position.x + assumed_player_half))
                        {
                            game.player.position.x = 1.0f - assumed_player_half;
                        }

                        if (-1.0f > (game.player.position.y - assumed_player_half))
                        {
                            game.player.position.y = -1.0f + assumed_player_half;
                        }
                        else if (1.0f < (game.player.position.y + assumed_player_half))
                        {
                            game.player.position.y = 1.0f - assumed_player_half;
                        }
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
                // Fill in with useful code
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

    DrawRectangleV(world_draw_position, {world_draw_space, world_draw_space}, MAROON);

    float draw_scale = world_draw_space / ASSUMED_WORLD_SIZE;

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

    
    std::stringstream draw_scale_sstr;
    draw_scale_sstr << "DRAW_SCALE := " << draw_scale;
    DrawText(draw_scale_sstr.str().c_str(), COUNTDOWN_DRAW_WORLD_WIDTH_RATIO * g_window_data.width + 16, 1, 12, BLACK);
    
    std::stringstream player_position_sstr;
    player_position_sstr << "PLAYER_POSITION := {" << (uint32_t)player.x << ", " << (uint32_t)player.y << "}";
    DrawText(player_position_sstr.str().c_str(), COUNTDOWN_DRAW_WORLD_WIDTH_RATIO * g_window_data.width + 16, 16, 12, BLACK);
    
    std::stringstream game_player_position_sstr;
    game_player_position_sstr << "PLAYER_POSITION := {" << game.player.position.x << ", " << game.player.position.y << "}";
    DrawText(game_player_position_sstr.str().c_str(), COUNTDOWN_DRAW_WORLD_WIDTH_RATIO * g_window_data.width + 16, 32, 12, BLACK);

    std::stringstream draw_size_sstr;
    draw_size_sstr << "PLAYER_SIZE := " << (uint32_t)assumed_player_size;
    DrawText(draw_size_sstr.str().c_str(), COUNTDOWN_DRAW_WORLD_WIDTH_RATIO * g_window_data.width + 16, 48, 12, BLACK);

}
