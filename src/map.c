#include "map.h"
#include "sprite_struct.h"
#include "bmp.h"

#include <stdlib.h>
#include <assert.h>


typedef struct MapStruct {
	Sprite sprite;
} MapStruct;


Sprite mapLoadSprite (Map map, char *imagePath, char **errorMessage) {
	Image image;
	Sprite sprite;

	image = loadBmp(imagePath, true, errorMessage);
	if (!image) {
		return false;
	}

	sprite = makeSprite(image);
	freeImage(image);

	return sprite;
}


Map makeMap (char **errorMessage) {
    Map map = malloc(sizeof(MapStruct));
	assert(map);

    map->sprite = mapLoadSprite(map, "../images/backgr.bmp", errorMessage);
	if (!map->sprite) {
		return NULL;
	}

	return map;
}


void freeMap (Map map) {
    freeSprite(map->sprite);
}


BitplaneStrip getMapStripAtWorldCoord(Map map, StripCoord worldCoord) {
    Sprite backgroundImage = map->sprite;
	unsigned short int sourceStripColumn;
	unsigned short int sourceStripY;
	unsigned short int sourceStripIndex;

	sourceStripColumn = worldCoord.column % backgroundImage->numColumns;
	sourceStripY = worldCoord.y % backgroundImage->height;

	sourceStripIndex = sourceStripColumn + sourceStripY * backgroundImage->numColumns;

	return backgroundImage->bitPlaneStrips[sourceStripIndex];
}
