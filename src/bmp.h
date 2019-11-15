#define bool char
#define true 1
#define false 0

typedef struct Image {
    unsigned int numColumns;
    unsigned int height;
	bool upsideDown;
    unsigned char palette[16];
    unsigned long int *pixels;
} Image;

bool loadBmp (Image *image, char* imageFilePath);
void freeImage (Image image);
