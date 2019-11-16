#ifndef ega_h
#define ega_h

void waitForFrame ();
void setVideoMode ();
void drawPoint (unsigned short int x, unsigned short int y, unsigned char color);
long unsigned int makeBitplaneStrip (unsigned long int nibbleStrip);
void drawStrip (short int column, short int y, unsigned long int strip, unsigned char mask);
void copyStrip (short int column, short int y);
void pasteStrip (short int column, short int y, unsigned char mask);
void setPalette (unsigned char palette[16]);

#endif
