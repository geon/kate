#include "sprite_instance.h"


defineVector(SpriteInstanceVector, SpriteInstance, spriteInstanceVector)

SpriteInstance makeSpriteInstance (Sprite sprite, int16_t posX, int16_t posY) {
	SpriteInstance spriteInstance;

	spriteInstance.sprite = sprite;
	spriteInstance.posX = posX;
	spriteInstance.posY = posY;

	return spriteInstance;
}
