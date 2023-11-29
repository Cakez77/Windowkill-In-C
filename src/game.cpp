#include "game.h"

#include "assets.h"
#include "texts.h"

// #############################################################################
//                           Game Constants
// #############################################################################

// #############################################################################
//                           Game Structs
// #############################################################################
struct SpawnBatch
{
  float startTime;
  float endTime;
  float frequency;
  float timer;
};

// #############################################################################
//                           Game Globals
// #############################################################################
static float gameTime = 0;
static SpawnBatch spawns[] = 
{
  {0, 60, 5}, // Normal Enemies, 1 every 5 Seconds
};

// #############################################################################
//                           Game Functions
// #############################################################################
bool just_pressed(GameInputType type)
{
  KeyMapping mapping = gameState->keyMappings[type];
  for(int idx = 0; idx < mapping.keys.count; idx++)
  {
    if(input->keys[mapping.keys[idx]].justPressed)
    {
      return true;
    }
  }

  return false;
}

bool is_down(GameInputType type)
{
  KeyMapping mapping = gameState->keyMappings[type];
  for(int idx = 0; idx < mapping.keys.count; idx++)
  {
    if(input->keys[mapping.keys[idx]].isDown)
    {
      return true;
    }
  }

  return false;
}


// #############################################################################
//                           Game Functions(exposed)
// #############################################################################
EXPORT_FN void update_game(GameState* gameStateIn, 
                           RenderData* renderDataIn, 
                           Input* inputIn, 
                           SoundState* soundStateIn,
                           UIState* uiStateIn,
                           float dt)
{
  if(renderData != renderDataIn)
  {
    gameState = gameStateIn;
    renderData = renderDataIn;
    input = inputIn;
    soundState = soundStateIn;
    uiState = uiStateIn;

    init_strings();
  }

  if(!gameState->initialized)
  {
    // Key Mappings
    {
      gameState->keyMappings[MOVE_UP].keys.add(KEY_W);
      gameState->keyMappings[MOVE_UP].keys.add(KEY_UP);
      gameState->keyMappings[MOVE_LEFT].keys.add(KEY_A);
      gameState->keyMappings[MOVE_LEFT].keys.add(KEY_LEFT);
      gameState->keyMappings[MOVE_DOWN].keys.add(KEY_S);
      gameState->keyMappings[MOVE_DOWN].keys.add(KEY_DOWN);
      gameState->keyMappings[MOVE_RIGHT].keys.add(KEY_D);
      gameState->keyMappings[MOVE_RIGHT].keys.add(KEY_RIGHT);
      gameState->keyMappings[MOUSE_LEFT].keys.add(KEY_MOUSE_LEFT);
      gameState->keyMappings[MOUSE_RIGHT].keys.add(KEY_MOUSE_RIGHT);
      gameState->keyMappings[PAUSE].keys.add(KEY_ESCAPE);
    }

    gameState->playerAttack = 20;
    gameState->windowStretch = 40;
    gameState->playerPos = {1920.0f / 2.0f, 1080.0f / 2.0f};
    gameState->initialized = true;
  }

  gameTime += dt;

  // Top Left Origin Point
  renderData->gameCamera.dimensions = vec_2(input->screenSize);
  renderData->gameCamera.position.x = input->screenSize.x / 2.0f + input->windowPos.x + 4;
  renderData->gameCamera.position.y = -input->screenSize.y / 2.0f - input->windowPos.y;

  // Top Left Origin Point
  renderData->uiCamera.dimensions = vec_2(input->screenSize);
  renderData->uiCamera.position.x = input->screenSize.x / 2.0f + input->windowPos.x + 4;
  renderData->uiCamera.position.y = -input->screenSize.y / 2.0f - input->windowPos.y;

  const float windowSizeXStart = 1920;
  const float windowSizeXEnd = 300;
  const float windowSizeYStart = 1080;
  const float windowSizeYEnd = 300;
  const float windowXStart = 0;
  const float windowYStart = 0;
  const float windowXEnd = (windowSizeXStart - windowSizeXEnd) / 2;
  const float windowYEnd = (windowSizeYStart - windowSizeYEnd) / 2;

  if(!gameState->roundInitialized)
  {
    static float timer = 0;
    const float duration = 5;
    timer += dt;

    if(timer < 2)
    {
      return;
    }

    if(timer >= duration)
    {
      gameState->roundInitialized = true;
    }

    float t = ease_out_back((timer - 2) / (duration - 2));

    input->windowRect.pos.x = windowXStart * (1 - t) + windowXEnd * t;
    input->windowRect.pos.y = windowYStart * (1 - t) + windowYEnd * t;
    input->windowRect.size.x = windowSizeXStart * (1 - t) + windowSizeXEnd * t;
    input->windowRect.size.y = windowSizeYStart * (1 - t) + windowSizeYEnd * t;

    return;
  }

  // play_sound("First Steps", SOUND_OPTION_LOOP);

  const float pixelsPerSecond = dt * 40;
  const float slowdown = 0.3;
  const float speed = 100;

  if(just_pressed(MOUSE_LEFT))
  {
    Projectile proj = 
    {
      .pos = gameState->playerPos,
      .direction = normalize(vec_2(input->mousePosScreen) - gameState->playerPos),
    };

   gameState->projectiles.add(proj);
  }

  if(is_down(MOVE_LEFT))
  {
    gameState->playerPos.x -= speed * dt;
  }

  if(is_down(MOVE_RIGHT))
  {
    gameState->playerPos.x += speed * dt;
  }

  if(is_down(MOVE_UP))
  {
    gameState->playerPos.y -= speed * dt;
  }

  if(is_down(MOVE_DOWN))
  {
    gameState->playerPos.y += speed * dt;
  }

  // TODO: Screen == World, for player position
  gameState->playerPos.x = clamp(gameState->playerPos.x, 
                                 input->windowRect.pos.x, 
                                 input->windowRect.pos.x + 
                                 input->windowRect.size.x);
  gameState->playerPos.y = clamp(gameState->playerPos.y, 
                                 input->windowRect.pos.y, 
                                 input->windowRect.pos.y + 
                                 input->windowRect.size.y - 6);

  // Vec2 moveStrength = Vec2{input->windowRect.size.x - windowSizeXEnd, 
  //                           input->windowRect.size.y - windowSizeYEnd} / 
  //                     Vec2{windowSizeXStart - windowSizeXEnd, 
  //                          windowSizeYStart - windowSizeYEnd};
  // moveStrength.x = ease_out_expo(moveStrength.x * 4);
  // moveStrength.y = ease_out_expo(moveStrength.y * 4);

  // draw_format_ui_text("moveStrength: %.2f", gameState->playerPos, moveStrength);

  float xDiff = fabsf(input->windowRect.size.x - windowSizeXEnd);
  float yDiff = fabsf(input->windowRect.size.y - windowSizeYEnd);
  Vec2 moveStrength = Vec2{xDiff / (windowSizeXStart - windowSizeXEnd), 
                           yDiff / (windowSizeYStart - windowSizeYEnd)};
  moveStrength.x = ease_out_expo(moveStrength.x * 4);
  moveStrength.y = ease_out_expo(moveStrength.y * 4);

  static Rect targetWindowRect = input->windowRect;
  // Window shrinking
  {
    targetWindowRect.pos.x += pixelsPerSecond * moveStrength.x;
    targetWindowRect.pos.y += pixelsPerSecond * moveStrength.y;
    targetWindowRect.size.x -= pixelsPerSecond * 2 * moveStrength.x;
    targetWindowRect.size.y -= pixelsPerSecond * 2 * moveStrength.y;
  }

  targetWindowRect.size.x = clamp(targetWindowRect.size.x, windowSizeXEnd, windowSizeXStart);
  targetWindowRect.size.y = clamp(targetWindowRect.size.y, windowSizeYEnd, windowSizeYStart);

  float t = 0.1f;
  input->windowRect.pos.x = lerp(input->windowRect.pos.x, targetWindowRect.pos.x, t);
  input->windowRect.pos.y = lerp(input->windowRect.pos.y, targetWindowRect.pos.y, t);
  input->windowRect.size.x = lerp(input->windowRect.size.x, targetWindowRect.size.x, t); 
  input->windowRect.size.y = lerp(input->windowRect.size.y, targetWindowRect.size.y, t);

  draw_sprite(SPRITE_PLAYER, gameState->playerPos,
              {.renderOptions = RENDERING_OPTION_TRANSPARENT});

  // Update and draw projectiles
  {
    const float projSpeed = 300;
    for(int projIdx = 0; projIdx < gameState->projectiles.count; projIdx++)
    {
      Projectile& proj = gameState->projectiles[projIdx];
      proj.pos += proj.direction * projSpeed * dt;

      // Left Collision with Window
      if(proj.pos.x <= input->windowRect.pos.x)
      {
        targetWindowRect.pos.x -= gameState->windowStretch;
        targetWindowRect.size.x += gameState->windowStretch;
        gameState->projectiles.remove_idx_and_swap(projIdx--);
        continue;
      }

      // Right Collision with Window
      if(proj.pos.x >= input->windowRect.pos.x + input->windowRect.size.x)
      {
        targetWindowRect.size.x += gameState->windowStretch;
        gameState->projectiles.remove_idx_and_swap(projIdx--);
        continue;
      }

      // Top Collision with Window
      if(proj.pos.y <= input->windowRect.pos.y)
      {
        targetWindowRect.pos.y -= gameState->windowStretch;
        targetWindowRect.size.y += gameState->windowStretch;
        gameState->projectiles.remove_idx_and_swap(projIdx--);
        continue;
      }

      // Bottom Collision with Window
      if(proj.pos.y >= input->windowRect.pos.y + input->windowRect.size.y)
      {
        targetWindowRect.size.y += gameState->windowStretch;
        gameState->projectiles.remove_idx_and_swap(projIdx--);
        continue;
      }

      bool enemyHit = false;
      for(int enemyIdx = 0; enemyIdx < gameState->enemies.count; enemyIdx++)
      {
        Enemy& enemy = gameState->enemies[enemyIdx];
        Rect enemyRect = {enemy.pos, {32, 38}};

        if(point_in_rect(proj.pos, enemyRect))
        {
          enemyHit = true;
          enemy.health -= gameState->playerAttack;
          if(enemy.health <= 0)
          {
            gameState->enemies.remove_idx_and_swap(enemyIdx);
          }
          break;
        }
      }

      if(enemyHit)
      {
        gameState->projectiles.remove_idx_and_swap(projIdx--);
        continue;
      }

      // Draw
      draw_quad(proj.pos, {8, 8});
    }
  }

  // Spawn System
  {
    Sprite sprite = get_sprite(SPRITE_ENEMY);
    for(int batchIdx = 0; batchIdx < ArraySize(spawns); batchIdx++)
    {
      SpawnBatch& batch = spawns[batchIdx];

      if(gameTime >= batch.startTime && gameTime <= batch.endTime)
      {
        batch.timer += dt;

        while(batch.timer >= batch.frequency)
        {
          Vec2 pos = {};
          float x = random_range(-100, 100);
          if(x < 0)
          {
            pos.x = x - sprite.size.x / 2.0f;
          }
          else
          {
            pos.x = input->windowRect.size.x + x + sprite.size.x / 2.0f;
          }

          float y = random_range(-100, 100);
          if(y < 0)
          {
            pos.y = y - sprite.size.y  / 2.0f;
          }
          else
          {
            pos.y = input->windowRect.size.y + y + sprite.size.y / 2.0f;
          }

          Enemy enemy = {};
          enemy.speed = speed;
          enemy.pos = pos;
          gameState->enemies.add(enemy);

          batch.timer -= batch.frequency;
        }
      }
    }
  }

  // Update and draw Enemies
  {
    const float enemySpeed = 100;
    for(int enemyIdx = 0; enemyIdx < gameState->enemies.count; enemyIdx++)
    {
      Enemy& enemy = gameState->enemies[enemyIdx];

      Vec2 direction = normalize(gameState->playerPos - enemy.pos);
      enemy.pos += direction * enemySpeed * dt;
      draw_sprite(SPRITE_ENEMY, enemy.pos);
    }
  }
}