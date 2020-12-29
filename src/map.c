#include "map.h"
#include "image_struct.h"
#include "bmp.h"
#include "strip_coord.h"
#include "uint16_vector.h"

#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdint.h>


typedef struct MapStruct {
	uint16_t numColumns;
	uint16_t height;
	uint8_t palette[16];
	BitplaneStripVectorStruct bitplaneStrips;
	Uint16VectorStruct bitplaneStripIndices;
} MapStruct;


bool bitplaneStripEquals(BitplaneStrip* a, BitplaneStrip* b) {
	return bitplaneStripAsInt(*a) == bitplaneStripAsInt(*b);
}


Map makeMap (char **errorMessage) {
	Image image;
	Map map = malloc(sizeof(MapStruct));
	assert(map);

	image = loadBmp("../images/backgr.bmp", true, errorMessage);
	if (!image) {
		return false;
	}

	map->numColumns = image->numColumns;
	map->height = image->height;

	memcpy(map->palette, image->palette, sizeof(map->palette));

	{
		uint16_t numBitplaneStripIndices = map->numColumns * map->height;
		initializeUint16Vector(&map->bitplaneStripIndices, numBitplaneStripIndices);
		// TODO: Hardcode or calculate a more reasonable capacity.
		initializeBitplaneStripVector(&map->bitplaneStrips, 64);

		{
			uint16_t i;
			for (i=0; i<numBitplaneStripIndices; ++i) {
				BitplaneStrip strip;
				int16_t stripIndex;
				uint16_t stripColumn, stripY;
				uint16_t imageY = stripY;

				stripColumn = i % image->numColumns;
				stripY = i / image->numColumns;
				if (image->upsideDown) {
					imageY = image->height - 1 - imageY;
				}
				strip = makeBitplaneStrip(image->pixels[stripColumn + imageY*image->numColumns]);

				stripIndex = bitplaneStripVectorIndexOf(&map->bitplaneStrips, strip, bitplaneStripEquals);
				if (stripIndex == -1) {
					bitplaneStripVectorPush(&map->bitplaneStrips, strip);
					stripIndex = bitplaneStripVectorSize(&map->bitplaneStrips) - 1;
				}
				uint16VectorPush(&map->bitplaneStripIndices, stripIndex);
			}
		}
	}

	freeImage(image);

	return map;
}


void freeMap (Map map) {
    destroyBitplaneStripVector(&map->bitplaneStrips);
}


uint16_t getMapStripAtWorldCoord(Map map, StripCoord worldCoord) {
	uint16_t sourceX;
	uint16_t sourceY;
	uint16_t sourceIndex;

	sourceX = worldCoord.column % map->numColumns;
	sourceY = worldCoord.y % map->height;

	sourceIndex = sourceX + sourceY * map->numColumns;

	return map->bitplaneStripIndices.values[sourceIndex];
}

BitplaneStrip *mapGetStrips (Map map) {
	return 	map->bitplaneStrips.values;
}
