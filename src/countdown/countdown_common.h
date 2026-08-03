#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <vector>

#include "raylib.h"

typedef struct STRUCT_WELCOME_DATA
{
    bool is_start_hovered = false;
    bool is_start_pressed = false;

    bool is_settings_hovered = false;
    bool is_settings_pressed = false;

    bool is_quit_hovered = false;
    bool is_quit_pressed = false;
    
    bool is_back_hovered = false;
    bool is_back_pressed = false;
} WelcomeData;

typedef struct STRUCT_PLAYER_DATA
{
    Vector2 position = {0.0f, 0.0f};
    Vector2 velocity = {0.0f, 0.0f};

    float speed = 300.0f;
} PlayerData;

typedef struct STRUCT_GAZUMPA_DATA
{
    Vector2 position = {0.0f, 0.0f};
    Vector2 velocity = {0.0f, 0.0f};

    Vector2 direction = {0.0f, 0.0f};
    Vector2 target    = {0.0f, 0.0f};

    float speed = 200.0f;
} GazumpaData;

enum class KligState
{
    CountingUp,
    Grabbed,
    CountingDown,
    Safe,
    Overloaded,

    KLIG_STATE_CNT
};

enum class KligTribes : uint8_t
{
    Anstral = 0,
    Behanas,
    Caboogla,
    Denafigie,
    Egaloop,
    Fenagak,
    Gedantino,
    Helipso,
    Isanta,
    Jabapok,
    Kagaroth,
    Linta,
    Moonag,
    Nobani,
    Ooodentu,
    Pilin,
    Quintri,
    Relath,
    Stoopid,
    Thavala,
    Ubug,
    Vernagok,
    Welamaji,
    Xenarik,
    Yelosa,
    Zedak,

    KLIG_TRIBE_CNT
};
typedef typename std::underlying_type<KligTribes>::type uKligTribes;

typedef struct STRUCT_KLIG_DATA
{
    Vector2    position       = {0.0f, 0.0f};
    Vector2    start_position = { 0.0f, 0.0f };
    KligTribes tribe = KligTribes::Anstral;
    Color      color;
    
    bool is_grabbed = false;
    bool is_home = false;

    bool is_grabbable = false;
    bool is_homable   = false;

    uint16_t count = 0;
    float    last_count_at = 0.0f;

    KligState state = KligState::CountingUp;
} KligData;

typedef struct STRUCT_ZONE_DATA
{
    Vector2 position = {0.0f, 0.0f};
    Vector2 size = {44.0f, 44.0f};

    KligTribes tribe = KligTribes::Anstral;
    Color      color = RED;

    enum class ZoneType
    {
        Home,
        Sticky,
        Blocker,

        ZONE_TYPE_CNT
    } type = ZoneType::Home;
} ZoneData;

typedef struct STRUCT_LEVEL_DATA
{
    double klig_count_rate = 1.0;

    Vector2 player_start_position = {42.0f, 420.0f};

    bool player_failed = false;
    bool player_success = false;

    std::vector<GazumpaData> gazumpas = {};
    std::vector<KligData>    kligs = {};
    std::vector<ZoneData>    zones = {};
} LevelData;

enum class GameState
{
    Welcome,
    Settings,
    Playing,
    Paused,
    GameOver,
    HighScores,

    GAME_STATE_CNT
};

typedef struct STRUCT_GAME_SETTINGS
{
    bool enable_vsync  = false;
    bool display_debug = false;

    bool is_back_hovered = false;
    bool is_back_pressed = false;
} GameSettings;

typedef struct STRUCT_GAME_USER_INTPUTS
{
    Vector2 mouse_position = {0.0f, 0.0f};
    bool    mouse_left_pressed = false;
    bool    mouse_right_pressed = false;

    bool kb_grab_pressed       = false;
    bool kb_paused_pressed     = false;
    bool kb_move_up_pressed    = false;
    bool kb_move_down_pressed  = false;
    bool kb_move_left_pressed  = false;
    bool kb_move_right_pressed = false;

    Vector2 gpad_movement       = {0.0f, 0.0f};
    bool    gpad_grab_pressed   = false;
    bool    gpad_paused_pressed = false;

    bool prev_mouse_left_pressed  = false;
    bool prev_mouse_right_pressed = false;
    bool prev_kb_grab_pressed     = false;
    bool prev_kb_paused_pressed   = false;
    bool prev_gpad_grab_pressed   = false;
    bool prev_gpad_paused_pressed = false;
} UserInputs;

typedef struct STRUCT_GAME_DATA
{
    GameState state = GameState::Welcome;

    UserInputs inputs;
    UserInputs prev_inputs;

    WelcomeData welcome_data;

    PlayerData player;

    std::vector<LevelData> levels;
    
    uint32_t level = 0;
    float    game_time = 0.0f;

    uint32_t levels_completed = 0;

    float    safe_at     = 0.0f;
    float    gameover_at = 0.0f;
} GameData;

void back_button_draw(Vector2 position, bool is_hovered);
void cursor_draw(Vector2 position);

constexpr uint32_t CURSOR_SIZE                = 12;
constexpr uint32_t BACK_BUTTON_HEIGHT         = 48;
constexpr uint32_t BACK_BUTTON_WIDTH          = 96;
constexpr uint32_t BACK_BUTTON_TEXT_SIZE      = 24;
constexpr uint32_t ASSUMED_WORLD_SIZE         = 1024;
constexpr uint32_t COUNTDOWN_DRAW_WRLD_BORDER = 64;
constexpr uint32_t PLAYER_SIZE                = 32;
constexpr uint32_t GAZUMPA_SIZE               = 24;
constexpr uint32_t KLIG_SIZE                  = 10;

extern GameSettings g_settings;
