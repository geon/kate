#include "platform/dos/ega.h"
#include "world.h"

#include <stdio.h>


int main (int argc, char* argv[]) {

	unsigned int frame;
	World world;

	setVideoMode();

	world = makeWorld();
	setPalette(getWorldPalette(world));
	setVirtualScreenWidth(EGA_BUFFER_NUM_COLUMNS);

	for (frame=0; frame < 100; ++frame) {
		updateWorld(world);
		renderWorld(world);
		waitForFrame();
	}

	// Restore screen.
	setScroll(0, 0);
	setVirtualScreenWidth(EGA_BUFFER_NUM_COLUMNS_DEFAULT);

	printf("\n\n\ndone");
	return 0;
}
