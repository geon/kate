#include "sprite_shift_cache.h"


defineVector(SpriteShiftCacheVector, SpriteShiftCache, spriteShiftCacheVector)

SpriteShiftCache makeSpriteShiftCache (Sprite sprite) {
	SpriteShiftCache spriteShiftCache;

	uint8_t shift;
	spriteShiftCache.preShiftedSprites[0] = *sprite;
	for (shift=1; shift<8; ++shift) {
		initializeSpriteWithShiftedCopy(
			&spriteShiftCache.preShiftedSprites[shift],
			sprite,
			shift
		);
	}

	return spriteShiftCache;
}
