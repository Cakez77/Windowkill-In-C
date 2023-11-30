#pragma once

#include "input.h"
#include "schnitzel_lib.h"
#include "sound.h"
#include "render_interface.h"
#include "ui.h"

// #############################################################################
//                           Game Globals
// #############################################################################
constexpr float WORLD_WIDTH = 1920.0f;
constexpr float WORLD_HEIGHT = 1080.0f;

// #############################################################################
//                           Game Structs
// #############################################################################
enum GameInputType
{
  MOVE_LEFT,
  MOVE_RIGHT,
  MOVE_UP,
  MOVE_DOWN,
  JUMP,

  MOUSE_LEFT,
  MOUSE_RIGHT,

  PAUSE,

  GAME_INPUT_COUNT
};

struct KeyMapping
{
  Array<KeyCodeID, 3> keys;
};

struct Projectile
{
  Vec2 pos;
  Vec2 direction;
  bool hitEnemy = true;
};

struct Enemy
{
  int health = 60;
  float speed;
  Vec2 pos;
};

enum GameStateID
{
  GAME_STATE_MAIN_MENU,
  GAME_STATE_IN_LEVEL,
};

struct GameState
{
  GameStateID state;
  bool initialized = false;
  bool roundInitialized = false;

  Rect worldRect;

  float worldStretch;
  float playerAttack;
  Vec2 playerPos;
  Array<Enemy, 30> enemies;

  Array<Projectile, 100> projectiles;
  KeyMapping keyMappings[GAME_INPUT_COUNT];
};

// #############################################################################
//                           Game Globals
// #############################################################################
static GameState* gameState;

// #############################################################################
//                           Game Functions (Exposed)
// #############################################################################
extern "C"
{
  EXPORT_FN void update_game(GameState* gameStateIn, 
                             RenderData* renderDataIn, 
                             Input* inputIn, 
                             SoundState* soundStateIn,
                             UIState* uiStateIn,
                             float dt);
}