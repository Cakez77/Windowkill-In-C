#ifdef ENGINE // Inside Game / Engine
#pragma once

#include "schnitzel_lib.h"
#define vec2 Vec2
#define ivec2 IVec2
#define vec4 Vec4

// Inside Shader
#else 

#define BIT(i) 1 << i

// Inside Both
#endif 

// #############################################################################
//                           Rendering Constants
// #############################################################################
int RENDERING_OPTION_FLIP_X = BIT(0);
int RENDERING_OPTION_FLIP_Y = BIT(1);
int RENDERING_OPTION_FONT = BIT(2);
int RENDERING_OPTION_TRANSPARENT = BIT(3);

// #############################################################################
//                           Rendering Structs
// #############################################################################
struct Transform
{
  vec2 pos; // This is currently the Top Left!!
  vec2 size;
  ivec2 atlasOffset;
  ivec2 spriteSize;
  int renderOptions;
  int materialIdx;
  float angle;
  float layer;
};

struct Material
{
	// Operator inside the Engine to compare materials
#ifdef ENGINE 
  vec4 color = COLOR_WHITE;
  vec4 additiveColor;
	bool operator==(Material other)
	{
		return color == other.color && additiveColor == other.additiveColor;
	}
#else
  vec4 color;
  vec4 additiveColor;
#endif
};