#include "sprite_instance.h"


defineVector(SpriteInstanceVector, SpriteInstance, spriteInstanceVector)

SpriteInstance makeSpriteInstance (Sprite sprite, PixelCoord pos) {
	SpriteInstance spriteInstance;

	spriteInstance.sprite = makeSpriteShiftCache(sprite);
	spriteInstance.pos = pos;

	return spriteInstance;
}


BitplaneStrip mixStripsByMask (BitplaneStrip background, BitplaneStrip foreground, uint8_t mask) {
	BitplaneStrip result;

	uint8_t plane;
	for (plane=0; plane<4; ++plane) {
		result.planes[plane] = (background.planes[plane] & ~mask) | (foreground.planes[plane] & mask);
	}

	return result;
}


void  spriteInstanceRenderSprites (SpriteInstanceVector spriteInstances, Map map, Buffer buffer) {
	SpriteInstance *spriteInstance;
	PositionAndStripVectorStruct stripBatch;

	initializePositionAndStripVector(&stripBatch, 300);
	vectorForeach (spriteInstanceVectorBegin(spriteInstances), spriteInstanceVectorEnd(spriteInstances), spriteInstance) {
		uint8_t rest = spriteInstance->pos.x % 8;

		StripCoord pos;
		pos.column = spriteInstance->pos.x / 8;
		pos.y = spriteInstance->pos.y;

		{
			Sprite sprite = &spriteInstance->sprite.preShiftedSprites[rest];
			BitplaneStrip *strips = mapGetStrips(map);
			uint16_t y, column;
			for (y = 0; y < sprite->height; ++y) {
				uint16_t destinationStripIndex;
				StripCoord worldCoord;

				for (column=0; column<sprite->numColumns; ++column) {
					uint16_t sourceStripIndex = column + y*sprite->numColumns;
					BitplaneStrip strip = sprite->bitPlaneStrips.values[sourceStripIndex];
					uint8_t mask = sprite->mask.values[sourceStripIndex];

					worldCoord.column = pos.column + column;
					worldCoord.y = pos.y + y;
					destinationStripIndex = bufferMapWorldCoordToBufferIndex(
						buffer,
						worldCoord
					);

					if (mask) {
						BitplaneStrip backgroundStrip = strips[getMapStripAtWorldCoord(map, worldCoord)];
						PositionAndStrip posAndStrip;
						posAndStrip.pos=destinationStripIndex;
						posAndStrip.strip=mixStripsByMask(backgroundStrip, strip, mask);
						positionAndStripVectorPush(&stripBatch, posAndStrip);
					}
				}
			}

			{
				StripCoord topLeft, bottomRight;
				topLeft.column = pos.column;
				topLeft.y = pos.y;
				bottomRight.column = topLeft.column + sprite->numColumns;
				bottomRight.y = topLeft.y + sprite->height;

				bufferMarkRectangleAsDirty(buffer, topLeft, bottomRight, map);
			}
		}
		drawCustomStrips(positionAndStripVectorBegin(&stripBatch), positionAndStripVectorEnd(&stripBatch));
		positionAndStripVectorClear(&stripBatch);
	}
	destroyPositionAndStripVector(&stripBatch);
}
