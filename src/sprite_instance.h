#ifndef sprite_instance_h
#define sprite_instance_h

#include "sprite.h"

typedef struct SpriteInstance {
    Sprite sprite;
	short int posX;
	short int posY;
} SpriteInstance;

SpriteInstance makeSpriteInstance (Sprite sprite, short int posX, short int posY);

#endif
