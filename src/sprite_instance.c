#include "sprite_instance.h"


defineVector(SpriteInstanceVector, SpriteInstance, spriteInstanceVector)

SpriteInstance makeSpriteInstance (Sprite sprite, PixelCoord pos) {
	SpriteInstance spriteInstance;

	spriteInstance.sprite = makeSpriteShiftCache(sprite);
	spriteInstance.pos = pos;

	return spriteInstance;
}


void spriteInstanceDraw(SpriteInstance *spriteInstance, Map map, Buffer buffer, PositionAndStripVector stripBatch) {
	uint8_t rest = spriteInstance->pos.x % 8;

	StripCoord pos;
	pos.column = spriteInstance->pos.x / 8;
	pos.y = spriteInstance->pos.y;

	spriteDraw(&spriteInstance->sprite.preShiftedSprites[rest], pos, map, buffer, stripBatch);
}
