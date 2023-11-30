#pragma once

#include "schnitzel_lib.h"

// #############################################################################
//                           Assets Constants
// #############################################################################

// #############################################################################
//                           Assets Structs
// #############################################################################
enum SpriteID
{
  SPRITE_WHITE,
  SPRITE_PLAYER,
  SPRITE_ENEMY,
  SPRITE_CROSSHAIR,

  SPRITE_COUNT
};

struct Sprite
{
  IVec2 atlasOffset;
  IVec2 size;
  int frameCount = 1;
};

// #############################################################################
//                           Assets Functions
// #############################################################################
Sprite get_sprite(SpriteID spriteID)
{
  Sprite sprite = {};
  sprite.frameCount = 1;

  switch(spriteID)
  {
    case SPRITE_WHITE:
    {
      sprite.atlasOffset = {0, 0};
      sprite.size = {1, 1};
      break;
    }

    case SPRITE_PLAYER:
    {
      sprite.atlasOffset = {16, 0};
      sprite.size = {48, 48};
      break;
    }

    case SPRITE_ENEMY:
    {
      sprite.atlasOffset = {64, 0};
      sprite.size = {48, 48};
      break;
    }
    case SPRITE_CROSSHAIR:
    {
      sprite.atlasOffset = {0, 48};
      sprite.size = {32, 32};
      break;
    }
  }

  return sprite;
}
