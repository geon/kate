#ifndef sprite_h
#define sprite_h

#include "image.h"
#include "vector.h"
#include "pixel_coord.h"
#include "map.h"
#include "buffer.h"

#include <stdbool.h>

typedef struct SpriteStruct *Sprite;

Sprite makeSprite (Image image);
void freeSprite (Sprite sprite);
uint8_t * getSpritePalette(Sprite sprite);
void spriteDraw(Sprite sprite, PixelCoord pos, Map map, Buffer buffer);


declareVector(SpriteVector, Sprite, spriteVector)
defineVectorStruct(SpriteVector, Sprite, spriteVector)

#endif
