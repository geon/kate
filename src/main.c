#include "platform/dos/ega.h"
#include "world.h"

#include <stdio.h>


int main (int argc, char* argv[]) {
	char *errorMessage;
	unsigned int frame;
	World world;

	setVideoMode();

	world = makeWorld(&errorMessage);
	if (!world) {
		printf(errorMessage);
		return 1;
	}

	setPalette(getWorldPalette(world));
	setVirtualScreenWidth(EGA_BUFFER_NUM_COLUMNS);

	for (frame=0; frame < 100; ++frame) {
		updateWorld(world);
		renderWorld(world);
	}

	// Restore screen.
	setScroll(0, 0, false);
	setVirtualScreenWidth(EGA_BUFFER_NUM_COLUMNS_DEFAULT);

	freeWorld(world);

	printf("\n\n\ndone");
	return 0;
}
