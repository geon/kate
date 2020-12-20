#ifndef sprite_instance_h
#define sprite_instance_h

#include "sprite.h"

#include <stdint.h>


typedef struct SpriteInstance {
    Sprite sprite;
	int16_t posX;
	int16_t posY;
} SpriteInstance;

SpriteInstance makeSpriteInstance (Sprite sprite, int16_t posX, int16_t posY);

#endif
