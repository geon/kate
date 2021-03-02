#include "platform/dos/ega.h"
#include "renderer.h"
#include "world.h"
#include "buffer.h"

#include <stdio.h>
#include <stdint.h>


int16_t main (int16_t argc, char* argv[]) {
	Renderer renderer;
	World world;

	setVideoMode();

	{
		char *errorMessage;

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
	}

	setPalette(getRendererPalette(renderer));
	setVirtualScreenWidth(EGA_BUFFER_NUM_COLUMNS);

	{
		uint16_t frame;
		for (frame=0;; ++frame) {
			worldUpdate(world);
			worldRender(world);
		}
	}

	// Restore screen.
	setBufferOffset(0, 0);
	setVirtualScreenWidth(EGA_BUFFER_NUM_COLUMNS_DEFAULT);

	freeWorld(world);
	freeRenderer(renderer);

	printf("\n\n\ndone");
	return 0;
}
