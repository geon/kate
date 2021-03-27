#ifndef sprite_h
#define sprite_h

#include "image.h"
#include "vector.h"
#include "strip_coord.h"
#include "map.h"
#include "buffer.h"
#include "position_and_strip.h"

#include <stdbool.h>

typedef struct SpriteStruct *Sprite;

Sprite makeSprite (Image image);
void initializeSpriteWithShiftedCopy (Sprite sprite, Sprite originalSprite, uint8_t shift);
void freeSprite (Sprite sprite);
uint8_t * getSpritePalette(Sprite sprite);
void spriteDraw(Sprite sprite, StripCoord pos, Map map, Buffer buffer, PositionAndStripVector stripBatch);


declareVector(SpriteVector, Sprite, spriteVector)
defineVectorStruct(SpriteVector, Sprite, spriteVector)

#endif
