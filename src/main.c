#include "platform/dos/ega.h"
#include "world.h"

#include <stdio.h>


int main (int argc, char* argv[]) {

	unsigned int frame;
	World world;

	setVideoMode();

	world = makeWorld();
	setPalette(getWorldPalette(world));

	for (frame=0; frame < 100; ++frame) {
		updateWorld(world);
		renderWorld(world);
		waitForFrame();
	}

	printf("\n\n\ndone");
	return 0;
}
