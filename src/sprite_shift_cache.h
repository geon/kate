#ifndef sprite_shift_cache_h
#define sprite_shift_cache_h

#include "sprite.h"
#include "sprite_struct.h"
#include "pixel_coord.h"
#include "vector.h"

#include <stdint.h>
#include <stdlib.h>


typedef struct SpriteShiftCache {
    SpriteStruct preShiftedSprites[8];
} SpriteShiftCache;

SpriteShiftCache makeSpriteShiftCache (Sprite sprite);


declareVector(SpriteShiftCacheVector, SpriteShiftCache, spriteShiftCacheVector)
defineVectorStruct(SpriteShiftCacheVector, SpriteShiftCache, spriteShiftCacheVector)

#endif
