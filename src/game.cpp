#include "game.h"

#include "assets.h"
#include "texts.h"

// #############################################################################
//                           Game Constants
// #############################################################################

// #############################################################################
//                           Game Structs
// #############################################################################

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

    gameState->initialized = true;
  }

  renderData->gameCamera.dimensions = vec_2(input->screenSize) / 2.0f;
  renderData->gameCamera.position.x = 0;
  renderData->gameCamera.position.y = 0;

  renderData->uiCamera.dimensions = vec_2(input->screenSize) / 2.0f;
  renderData->uiCamera.position.x = 0;
  renderData->uiCamera.position.y = 0;

  const Vec2 MIN_WINDOW_SIZE = {640, 480};


  const float windowSizeXStart = 1920;
  const float windowSizeXEnd = 640;
  const float windowSizeYStart = 1080;
  const float windowSizeYEnd = 480;
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

  const float pixelsPerSecond = dt * 34;
  const float slowdown = 0.3;

  if(just_pressed(MOVE_LEFT))
  {
    input->forces.x = -4;
    input->forces.z =  2.2;
    // input->forces.z = 5 / 2;
  }

  if(just_pressed(MOVE_RIGHT))
  {
    input->forces.z = 3;
  }

  if(just_pressed(MOVE_UP))
  {
    input->forces.y = -4;
    input->forces.w =  2.2;
    // input->forces.z = 5 / 2;
  }

  if(just_pressed(MOVE_DOWN))
  {
    input->forces.w = 3;
  }

  Vec2 moveStrength = Vec2{input->windowRect.size.x - windowSizeXEnd, 
                            input->windowRect.size.y - windowSizeYEnd} / 
                      Vec2{windowSizeXStart - windowSizeXEnd, 
                           windowSizeYStart - windowSizeYEnd};  

  moveStrength.x = ease_out_expo(moveStrength.x * 4);
  moveStrength.y = ease_out_expo(moveStrength.y * 4);

  // draw_format_ui_text("windowSize.y")
  draw_format_ui_text("x:%.2f,y:%.2f", {-140, 0}, moveStrength.x, moveStrength.y);

  input->forces.x = approach(input->forces.x, pixelsPerSecond * moveStrength.x, slowdown);
  input->forces.y = approach(input->forces.y, pixelsPerSecond * moveStrength.y, slowdown);
  input->forces.z = approach(input->forces.z, -pixelsPerSecond * moveStrength.x, slowdown);
  input->forces.w = approach(input->forces.w, -pixelsPerSecond * moveStrength.y, slowdown);

  input->windowRect.pos.x += input->forces.x;
  input->windowRect.pos.y += input->forces.y;
  input->windowRect.size.x += input->forces.z * 2;
  input->windowRect.size.y += input->forces.w * 2;

  input->windowRect.pos.x = clamp(input->windowRect.pos.x, 0, windowSizeXStart - windowSizeXEnd);
  input->windowRect.pos.y = clamp(input->windowRect.pos.y, 0, windowSizeYStart - windowSizeYEnd);
  input->windowRect.size.x = clamp(input->windowRect.size.x, windowSizeXEnd, windowSizeXStart);
  input->windowRect.size.y = clamp(input->windowRect.size.y, windowSizeYEnd, windowSizeYStart);

  // draw_quad({100, 100}, {100, 100});
}