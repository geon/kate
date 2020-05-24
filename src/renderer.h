#ifndef renderer_h
#define renderer_h

#include "sprite_instance.h"
#include "map.h"

typedef struct RendererStruct *Renderer;

Renderer makeRenderer (char **errorMessage);
void freeRenderer (Renderer renderer);
unsigned char * getRendererPalette(Renderer renderer);
Sprite rendererLoadSprite (Renderer renderer, char *imagePath, char **errorMessage);
void rendererRender(Renderer renderer, unsigned int numSpriteInstances, SpriteInstance *spriteInstances, Map map);

#endif
