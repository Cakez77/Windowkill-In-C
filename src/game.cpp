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
  {0, 60, 4}, // Normal Enemies, 1 every 4 Seconds
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

    gameState->lives = 10;
    gameState->attackCooldown = 0.5;
    gameState->playerKnockback = 0;
    gameState->playerAttack = 20;
    gameState->worldStretch = 40;
    gameState->playerPos = {1920.0f / 2.0f, 1080.0f / 2.0f};
    gameState->initialized = true;
    gameState->worldRect = input->clientRect;
  }

  gameTime += dt;

  // Top Left Origin Point, 4 is windows window border / 2 (8px / 2)
  renderData->gameCamera.dimensions = input->clientRect.size;
  renderData->gameCamera.position.x = input->clientRect.size.x / 2.0f + input->clientRect.pos.x;
  renderData->gameCamera.position.y = -input->clientRect.size.y / 2.0f - input->clientRect.pos.y;

  // Top Left Origin Point
  renderData->uiCamera.dimensions = input->clientRect.size;
  renderData->uiCamera.position.x = input->clientRect.size.x / 2.0f + input->clientRect.pos.x;
  renderData->uiCamera.position.y = -input->clientRect.size.y / 2.0f - input->clientRect.pos.y;

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

    gameState->worldRect.pos.x = windowXStart * (1 - t) + windowXEnd * t;
    gameState->worldRect.pos.y = windowYStart * (1 - t) + windowYEnd * t;
    gameState->worldRect.size.x = windowSizeXStart * (1 - t) + windowSizeXEnd * t;
    gameState->worldRect.size.y = windowSizeYStart * (1 - t) + windowSizeYEnd * t;

    return;
  }

  // play_sound("First Steps", SOUND_OPTION_LOOP);

  const float pixelsPerSecond = dt * 40;
  const float slowdown = 0.3;
  const float speed = 100;

  draw_sprite(SPRITE_CROSSHAIR, input->mousePosScreen);

  gameState->attackTime = min(gameState->attackCooldown, 
                              gameState->attackTime + dt);
  if(is_down(MOUSE_LEFT))
  {

    while(gameState->attackTime >= gameState->attackCooldown)
    {
      Projectile proj = 
      {
        .pos = gameState->playerPos,
        .direction = normalize(input->mousePosScreen - gameState->playerPos),
      };

      play_sound("shoot");
      gameState->projectiles.add(proj);
      gameState->attackTime -= gameState->attackCooldown;
    }
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

  draw_format_ui_text("Gametime: %.2f", gameState->playerPos, gameTime);

  // TODO: Screen == World, for player position
  gameState->playerPos.x = clamp(gameState->playerPos.x, 
                                 gameState->worldRect.pos.x, 
                                 gameState->worldRect.pos.x + 
                                 gameState->worldRect.size.x);
  gameState->playerPos.y = clamp(gameState->playerPos.y, 
                                 gameState->worldRect.pos.y, 
                                 gameState->worldRect.pos.y + 
                                 gameState->worldRect.size.y - 6);

  // Vec2 moveStrength = Vec2{input->windowRect.size.x - windowSizeXEnd, 
  //                           input->windowRect.size.y - windowSizeYEnd} / 
  //                     Vec2{windowSizeXStart - windowSizeXEnd, 
  //                          windowSizeYStart - windowSizeYEnd};
  // moveStrength.x = ease_out_expo(moveStrength.x * 4);
  // moveStrength.y = ease_out_expo(moveStrength.y * 4);

  // draw_format_ui_text("moveStrength: %.2f", gameState->playerPos, moveStrength);

  float xDiff = fabsf(gameState->worldRect.size.x - windowSizeXEnd);
  float yDiff = fabsf(gameState->worldRect.size.y - windowSizeYEnd);
  Vec2 moveStrength = Vec2{xDiff / (windowSizeXStart - windowSizeXEnd), 
                           yDiff / (windowSizeYStart - windowSizeYEnd)};
  moveStrength.x = ease_out_expo(moveStrength.x * 4);
  moveStrength.y = ease_out_expo(moveStrength.y * 4);

  static Rect targetWorldRect = gameState->worldRect;
  // Window shrinking
  {
    targetWorldRect.pos.x += pixelsPerSecond * moveStrength.x;
    targetWorldRect.pos.y += pixelsPerSecond * moveStrength.y;
    targetWorldRect.size.x -= pixelsPerSecond * 2 * moveStrength.x;
    targetWorldRect.size.y -= pixelsPerSecond * 2 * moveStrength.y;
  }

  targetWorldRect.size.x = clamp(targetWorldRect.size.x, windowSizeXEnd, windowSizeXStart);
  targetWorldRect.size.y = clamp(targetWorldRect.size.y, windowSizeYEnd, windowSizeYStart);

  float t = 0.1f;
  gameState->worldRect.pos.x = lerp(gameState->worldRect.pos.x, targetWorldRect.pos.x, t);
  gameState->worldRect.pos.y = lerp(gameState->worldRect.pos.y, targetWorldRect.pos.y, t);
  gameState->worldRect.size.x = lerp(gameState->worldRect.size.x, targetWorldRect.size.x, t); 
  gameState->worldRect.size.y = lerp(gameState->worldRect.size.y, targetWorldRect.size.y, t);

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
      if(proj.pos.x <= input->clientRect.pos.x)
      {
        targetWorldRect.pos.x -= gameState->worldStretch;
        targetWorldRect.size.x += gameState->worldStretch;
        gameState->projectiles.remove_idx_and_swap(projIdx--);
        continue;
      }

      // Right Collision with Window
      if(proj.pos.x >= input->clientRect.pos.x + input->clientRect.size.x)
      {
        targetWorldRect.size.x += gameState->worldStretch;
        gameState->projectiles.remove_idx_and_swap(projIdx--);
        continue;
      }

      // Top Collision with Window
      if(proj.pos.y <= input->clientRect.pos.y)
      {
        targetWorldRect.pos.y -= gameState->worldStretch;
        targetWorldRect.size.y += gameState->worldStretch;
        gameState->projectiles.remove_idx_and_swap(projIdx--);
        continue;
      }

      // Bottom Collision with Window
      if(proj.pos.y >= input->clientRect.pos.y + input->clientRect.size.y)
      {
        targetWorldRect.size.y += gameState->worldStretch;
        gameState->projectiles.remove_idx_and_swap(projIdx--);
        continue;
      }

      bool enemyHit = false;
      for(int enemyIdx = 0; enemyIdx < gameState->enemies.count; enemyIdx++)
      {
        Enemy& enemy = gameState->enemies[enemyIdx];
        Rect enemyRect = {enemy.pos, {32, 38}};

        if(point_in_rect_center(proj.pos, enemyRect))
        {
          enemyHit = true;
          play_sound("impact");
          enemy.health -= gameState->playerAttack;
          enemy.pushTime = 0.25f;
          enemy.color = COLOR_WHITE;
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
  if(true)
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
          float x = random_range(-50, 50);
          if(x < 0)
          {
            pos.x = x - sprite.size.x / 2.0f;
          }
          else
          {
            pos.x = input->clientRect.size.x + x + sprite.size.x / 2.0f;
          }

          float y = random_range(-50, 50);
          if(y < 0)
          {
            pos.y = y - sprite.size.y  / 2.0f;
          }
          else
          {
            pos.y = input->clientRect.size.y + y + sprite.size.y / 2.0f;
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
      enemy.pushTime =max(0.0f, enemy.pushTime - dt);
      float pushForce = ease_in_quad(enemy.pushTime);
      float knockback = max(0.0f, gameState->playerKnockback - 
                                  enemy.knockbackResist);
      enemy.pos += direction * enemySpeed * dt - 
                   direction * pushForce * knockback;

      // Hit color management
      enemy.color.r = max(0.0f, enemy.color.r - dt * 10);
      enemy.color.g = max(0.0f, enemy.color.g - dt * 10);
      enemy.color.b = max(0.0f, enemy.color.b - dt * 10);
      enemy.color.a = max(0.0f, enemy.color.a - dt * 10);

      draw_sprite(SPRITE_ENEMY, enemy.pos, {.material{.additiveColor = enemy.color}});
    }
  }
}