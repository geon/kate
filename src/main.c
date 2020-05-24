#include "platform/dos/ega.h"
#include "renderer.h"
#include "world.h"
#include "buffer.h"

#include <stdio.h>


int main (int argc, char* argv[]) {
	char *errorMessage;
	unsigned int frame;
	Renderer renderer;
	World world;

	setVideoMode();

	renderer = makeRenderer(&errorMessage);
	if (!renderer) {
		printf(errorMessage);
		return 1;
	}

	world = makeWorld(renderer, &errorMessage);

	if (!world) {
		printf(errorMessage);
		return 1;
	}

	setPalette(getRendererPalette(renderer));
	setVirtualScreenWidth(EGA_BUFFER_NUM_COLUMNS);

	for (frame=0; frame < 100; ++frame) {
		worldUpdate(world);
		renderWorld(world);
	}

	// Restore screen.
	worldSetScroll(0, 0, false);
	setVirtualScreenWidth(EGA_BUFFER_NUM_COLUMNS_DEFAULT);

	freeWorld(world);
	freeRenderer(renderer);

	printf("\n\n\ndone");
	return 0;
}
