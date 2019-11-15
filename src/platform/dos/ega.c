#include "ega.h"

#include <dos.h>
#include <conio.h>
#include <sys\types.h>
#include <direct.h>
#define inportb(port) inp(port)

#define BIOS_INTERRUPT_VIDEO 0X10 // The BIOS video interrupt number.
#define VIDEO_MODE_EGA_640_350_16 0x10 // EGA 640 x 350, 16 colors.
	

void waitForFrame () {
	// Get the address of the input status register and crt controller.
	unsigned videoIsr = 0x03da;
	// Wait for a vertical retrace.
	while (!(inp(videoIsr) & 8)) {}
	// Wait for horizontal or vertical retrace.
	while (inp(videoIsr) & 1) {}
}


// Set vodeo mode to EGA 640x350 16c.
void setVideoMode () {
	union REGS regs;

	// service 0 - set current video mode
	regs.h.ah = 0;
	// requested mode
	regs.h.al = VIDEO_MODE_EGA_640_350_16;
	
	int86(BIOS_INTERRUPT_VIDEO, &regs, &regs);
}


void drawPoint (unsigned short int x, unsigned short int y, unsigned char color) {
	// For some reason, using the int86 function was very glitchy in DOSBox. Inline asm works fine.
	_asm {
		mov AH, 0x0C
		mov AL, color
		mov CX, x
		mov DX, y
		int 0x10
	}
}


#define BIT_0 1
#define BIT_1 2
#define BIT_2 4
#define BIT_3 8


/**
	Turn an array of 8 4-bit colors into 4 bit-planes, each with an 8-bit wide strip of pixels.
*/
long unsigned int makeBitplaneStrip (unsigned long int nibbleStrip) {

	unsigned long int 
		pixel0,
		pixel1,
		pixel2,
		pixel3,
		pixel4,
		pixel5,
		pixel6,
		pixel7,
		bitplane0,
		bitplane1,
		bitplane2,
		bitplane3;

	pixel0 = (nibbleStrip >> 28) & 0xf;
	pixel1 = (nibbleStrip >> 24) & 0xf;
	pixel2 = (nibbleStrip >> 20) & 0xf;
	pixel3 = (nibbleStrip >> 16) & 0xf;
	pixel4 = (nibbleStrip >> 12) & 0xf;
	pixel5 = (nibbleStrip >> 8) & 0xf;
	pixel6 = (nibbleStrip >> 4) & 0xf;
	pixel7 = (nibbleStrip >> 0) & 0xf;

	bitplane0 = (pixel0 & BIT_0) << 7 | (pixel1 & BIT_0) << 6 | (pixel2 & BIT_0) << 5 | (pixel3 & BIT_0) << 4 | (pixel4 & BIT_0) << 3 | (pixel5 & BIT_0) << 2 | (pixel6 & BIT_0) << 1 | (pixel7 & BIT_0) << 0;
	bitplane1 = (pixel0 & BIT_1) << 6 | (pixel1 & BIT_1) << 5 | (pixel2 & BIT_1) << 4 | (pixel3 & BIT_1) << 3 | (pixel4 & BIT_1) << 2 | (pixel5 & BIT_1) << 1 | (pixel6 & BIT_1) << 0 | (pixel7 & BIT_1) >> 1;
	bitplane2 = (pixel0 & BIT_2) << 5 | (pixel1 & BIT_2) << 4 | (pixel2 & BIT_2) << 3 | (pixel3 & BIT_2) << 2 | (pixel4 & BIT_2) << 1 | (pixel5 & BIT_2) << 0 | (pixel6 & BIT_2) >> 1 | (pixel7 & BIT_2) >> 2;
	bitplane3 = (pixel0 & BIT_3) << 4 | (pixel1 & BIT_3) << 3 | (pixel2 & BIT_3) << 2 | (pixel3 & BIT_3) << 1 | (pixel4 & BIT_3) << 0 | (pixel5 & BIT_3) >> 1 | (pixel6 & BIT_3) >> 2 | (pixel7 & BIT_3) >> 3;

	return bitplane0 << 0 | bitplane1 << 8 | bitplane2 << 16 | bitplane3 << 24;
}


void drawStrip (short int column, short int y, unsigned long int strip, unsigned char mask) {

	/*
	write modes:
		* 0. The latch is ignored. "data unmodiﬁed" (latch, must still be loaded if bit mask is used?)
		* 1. Write the latched pixels. the byte written by the cpu are irrelevant.

	2 and 3 are or and and resp. Can be used to clear certain pixeld with and, then fill them with or.
	*/

	short int stripOffset = y*80 + column;

	unsigned char *bitStrips = (unsigned char *) &strip;

	unsigned char bitplane0 = bitStrips[0];
	unsigned char bitplane1 = bitStrips[1];
	unsigned char bitplane2 = bitStrips[2];
	unsigned char bitplane3 = bitStrips[3];

	const unsigned char allBits = BIT_0 | BIT_1 | BIT_2 | BIT_3;

	_asm{
		; Set write mode
			mov dx, 03CEh ; 6845 command register
			; Specify mode register
			mov al, 5
			; Just write. Ignore latch.
			mov ah, 0
			; Send
			out dx, ax

		; Set 6845 bit mask register
			; Specify bit mask register
			mov al, 8
			mov ah, mask
			; Send bit mask
			out dx, ax

		; Point ES to video memory segment
			mov ax, 0A000h
			mov es, ax

		; Enable plane 0
			; 6845 command register
			mov dx, 3C4h
			; Specify sequencer register
			mov al, 2
			; only plane 0
			mov ah, BIT_0
			; Send
			out dx, ax

		; Draw the strip
			mov bx, stripOffset
			; Get the pixel value
			mov al, bitplane0
			; Write the pixel
			mov es:[bx], al

		; Enable plane 1
			; 6845 command register
			mov dx, 3C4h
			; Specify mode register
			mov al, 2
			; only plane 1
			mov ah, BIT_1
			; Send
			out dx, ax

		; Draw the strip
			mov bx, stripOffset
			; Get the pixel value
			mov al, bitplane1
			; Write the pixel
			mov es:[bx], al

		; Enable plane 2
			; 6845 command register
			mov dx, 3C4h
			; Specify mode register
			mov al, 2
			; only plane 2
			mov ah, BIT_2
			; Send
			out dx, ax

		; Draw the strip
			mov bx, stripOffset
			; Get the pixel value
			mov al, bitplane2
			; Write the pixel
			mov es:[bx], al

		; Enable plane 3
			; 6845 command register
			mov dx, 3C4h
			; Specify mode register
			mov al, 2
			; only plane 3
			mov ah, BIT_3
			; Send
			out dx, ax

		; Draw the strip
			mov bx, stripOffset
			; Get the pixel value
			mov al, bitplane3
			; Write the pixel
			mov es:[bx], al

		; Re-enable all planes
			; 6845 command register
			mov dx, 3C4h
			; Specify mode register
			mov al, 2
			; bits 0-3.
			mov ah, allBits
			; Send
			out dx, ax
	}
}


void copyStrip (short int column, short int y) {
	short int stripOffset = y*80 + column;

	_asm{
		; Point ES to video memory segment
			mov ax, 0A000h
			mov es, ax

		; Load the strip
			mov bx, stripOffset
			; Load 6845 latch registers. It stays in the latch. The value read to the register is irrelevant.
			mov al, es:[bx]
	}
}

void pasteStrip (short int column, short int y, unsigned char mask) {
	short int stripOffset = y*80 + column;

	_asm{
		; Set write mode
			; 6845 command register
			mov dx, 03CEh
			; Specify mode register
			mov al, 5
			; Write from latch.
			mov ah, 1
			; Send
			out dx, ax

		; Set 6845 bit mask register
			; Specify bit mask register
			mov al, 8
			mov ah, mask
			; Send bit mask
			out dx, ax

		; Point ES to video memory segment
			mov ax, 0A000h
			mov es, ax

		; Draw the strip
			mov bx, stripOffset
			; Write the pixel. The value is irrelevant.
			mov es:[bx], 0
	}
}


void setPalette (unsigned char palette[16]) {
	union REGS regs;
	int i;

	// Service 0. Set an individual palette register.
	regs.x.ax = 0x1000;

	for (i=0; i<16; ++i) {
		// BL holds the palette index to be set. (0-15)
		regs.h.bl = i;
		// BH holds the new color.
		regs.h.bh = palette[i];
		// Tell BIOS to set it.
		int86(BIOS_INTERRUPT_VIDEO, &regs, &regs);
	}
}
