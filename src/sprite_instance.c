#include "sprite_instance.h"


defineVector(SpriteInstanceVector, SpriteInstance, spriteInstanceVector)

SpriteInstance makeSpriteInstance (Sprite sprite, PixelCoord pos) {
	SpriteInstance spriteInstance;

	spriteInstance.sprite = sprite;
	spriteInstance.pos = pos;

	return spriteInstance;
}
