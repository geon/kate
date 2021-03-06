#include "platform/dos/ega.h"
#include "renderer.h"
#include "world.h"
#include "buffer.h"

#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <math.h>

#define NUM_FRAMES 100


int16_t main (int16_t argc, char* argv[]) {
	clock_t startTime, stopTime, deltaTime;
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
		startTime = clock();
		for (frame=0; frame<NUM_FRAMES; ++frame) {
			worldUpdate(world);
			worldRender(world);
		}
		stopTime = clock();
	}

	// Restore screen.
	setBufferOffset(0, 0);
	setVirtualScreenWidth(EGA_BUFFER_NUM_COLUMNS_DEFAULT);

	deltaTime = stopTime - startTime;
	if (deltaTime) {
		printf("FPS: %i\n", NUM_FRAMES*CLOCKS_PER_SEC/deltaTime);
	}

	freeWorld(world);
	freeRenderer(renderer);

	printf("\n\n\ndone");
	return 0;
}
