#ifndef sprite_h
#define sprite_h

#include "image.h"
#include <stdbool.h>

typedef struct SpriteStruct *Sprite;

Sprite makeSprite (Image image);
void freeSprite (Sprite sprite);
uint8_t * getSpritePalette(Sprite sprite);

#endif
