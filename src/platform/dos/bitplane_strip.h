#ifndef bitplane_strip_h
#define bitplane_strip_h

typedef struct BitplaneStrip {
    unsigned char planes[4];
} BitplaneStrip;

BitplaneStrip makeBitplaneStrip (unsigned long int nibbleStrip);

#endif
