#include "map.h"
#include "image_struct.h"
#include "bmp.h"
#include "strip_tile.h"

#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdint.h>


typedef struct MapStruct {
	uint16_t width;
	uint16_t height;
	uint8_t palette[16];
	StripTileVectorStruct stripTiles;
} MapStruct;


Map makeMap (char **errorMessage) {
	Image image;
	Map map = malloc(sizeof(MapStruct));
	assert(map);

	image = loadBmp("../images/backgr.bmp", true, errorMessage);
	if (!image) {
		return false;
	}

	map->width = image->numColumns;
	map->height = image->height / 8;

	memcpy(map->palette, image->palette, sizeof(map->palette));

	{
		uint16_t numStripTiles = map->width * map->height;
		initializeStripTileVector(&map->stripTiles, numStripTiles);

		{
			uint16_t i;
			int16_t strip;
			for (i=0; i<numStripTiles; ++i) {
				StripTile tile;
				uint16_t tileX, tileY;
				tileX = i % image->numColumns;
				tileY = i / image->numColumns;
				for (strip=0; strip<8; ++strip) {
					uint16_t imageY = tileY * 8 + strip;
					if (image->upsideDown) {
						imageY = image->height - 1 - imageY;
					}
					tile.bitPlaneStrips[strip] = makeBitplaneStrip(image->pixels[tileX + imageY*image->numColumns]);
				}
				stripTileVectorPush(&map->stripTiles, tile);
			}
		}
	}

	freeImage(image);

	return map;
}


void freeMap (Map map) {
    destroyStripTileVector(&map->stripTiles);
}


BitplaneStrip getMapStripAtWorldCoord(Map map, StripCoord worldCoord) {
	uint16_t sourceTileX;
	uint16_t sourceTileY;
	uint16_t sourceTileIndex;

	sourceTileX = worldCoord.column % map->width;
	sourceTileY = (worldCoord.y/8) % map->height;

	sourceTileIndex = sourceTileX + sourceTileY * map->width;

	return map->stripTiles.values[sourceTileIndex].bitPlaneStrips[worldCoord.y%8];
}
