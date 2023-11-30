
// Input

// Output
layout (location = 0) out vec2 textureCoordsOut;
layout (location = 1) out flat int renderOptions;
layout (location = 2) out flat int materialIdx;

// Buffers
layout (std430, binding = 0) buffer TransformSBO
{
  Transform transforms[];
};

uniform vec2 screenSize;
uniform mat4 orthoProjection;


void main()
{
  Transform transform = transforms[gl_InstanceID];

  // Generating Vertices on the GPU
  // mostly because we have a 2D Engine

  // OpenGL Coordinates
  // -1/ 1                1/ 1
  // -1/-1                1/-1
  vec2 vertices[6] =
  {
    transform.pos,                                        // Top Left
    vec2(transform.pos + vec2(0.0, transform.size.y)),    // Bottom Left
    vec2(transform.pos + vec2(transform.size.x, 0.0)),    // Top Right
    vec2(transform.pos + vec2(transform.size.x, 0.0)),    // Top Right
    vec2(transform.pos + vec2(0.0, transform.size.y)),    // Bottom Left
    transform.pos + transform.size                        // Bottom Right
  };

  // Rotation
  {
    float angle = transform.angle;
    vec2 offset = transform.pos + transform.size / 2.0;
    
    for (int i = 0; i < 6; i++)
    {
      float newX = (vertices[i].x - offset.x) * cos(angle) - 
      (vertices[i].y - offset.y) * sin(angle);
      
      float newY = (vertices[i].x - offset.x) * sin(angle) + 
      (vertices[i].y - offset.y) * cos(angle);
      
      vertices[i].xy = vec2(newX + offset.x, newY + offset.y);
    }
  }

  int left = transform.atlasOffset.x;
  int top = transform.atlasOffset.y;
  int right = transform.atlasOffset.x + transform.spriteSize.x;
  int bottom = transform.atlasOffset.y + transform.spriteSize.y;

  if(bool(transform.renderOptions & RENDERING_OPTION_FLIP_X))
  {
    int tmp = left;
    left = right;
    right = tmp;
  }

  if(bool(transform.renderOptions & RENDERING_OPTION_FLIP_Y))
  {
    int tmp = top;
    top = bottom;
    bottom = tmp;
  }

  vec2 textureCoords[6] = 
  {
    vec2(left, top),
    vec2(left, bottom),
    vec2(right, top),
    vec2(right, top),
    vec2(left, bottom),
    vec2(right, bottom),
  };
 
  // Normalize Position
  {
    vec2 vertexPos = vertices[gl_VertexID];
    // vertexPos.y = -vertexPos.y + screenSize.y;
    // vertexPos = 2.0 * (vertexPos / screenSize) - 1.0;
    gl_Position = orthoProjection * vec4(vertexPos, transform.layer, 1.0);
  }

  textureCoordsOut = textureCoords[gl_VertexID];
  renderOptions = transform.renderOptions;
  materialIdx = transform.materialIdx;
}









