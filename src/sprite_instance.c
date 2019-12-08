#include "sprite_instance.h"


SpriteInstance makeSpriteInstance (Sprite sprite, short int posX, short int posY) {
	SpriteInstance spriteInstance;

	spriteInstance.sprite = sprite;
	spriteInstance.posX = posX;
	spriteInstance.posY = posY;

	return spriteInstance;
}
