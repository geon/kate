#include "ega.h"
#include "bitplane_strip.h"

#include <dos.h>
#include <conio.h>
#include <sys\types.h>
#include <direct.h>
#define inportb(port) inp(port)

#define BIOS_INTERRUPT_VIDEO 0X10 // The BIOS video interrupt number.
#define VIDEO_MODE_EGA_640_350_16 0x10 // EGA 640 x 350, 16 colors.

unsigned char *bufferBaseAddress = (unsigned char*) (0xa000 * 16);


void waitForFrame () {
	// Get the address of the input status register and crt controller.
	unsigned videoIsr = 0x03da;
	// Wait for a vertical retrace.
	while (!(inp(videoIsr) & 8)) {}
	// Wait for horizontal or vertical retrace.
	while (inp(videoIsr) & 1) {}
}


// Set video mode to EGA 640x350 16c.
void setVideoMode () {
	union REGS regs;

	// service 0 - set current video mode
	regs.h.ah = 0;
	// requested mode
	regs.h.al = VIDEO_MODE_EGA_640_350_16;
	
	int386(BIOS_INTERRUPT_VIDEO, &regs, &regs);
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


void drawStrip (unsigned short int index, BitplaneStrip bitplaneStrip, unsigned char mask) {

	/*
	write modes:
		* 0. Write the latched pixels, combined with CPU data as per register 3. Default is unmodified CPU/latch depending on mask.
		* 1. Write the latched pixels. The byte written by the cpu are irrelevant. (Not sure how this is different from mode 0 with a zeroed out mask.)
		* 2. Write a single color to all 8 pixels.
	*/

	unsigned char *stripAddress = bufferBaseAddress + index;

	unsigned char bitplane0 = bitplaneStrip.planes[0];
	unsigned char bitplane1 = bitplaneStrip.planes[1];
	unsigned char bitplane2 = bitplaneStrip.planes[2];
	unsigned char bitplane3 = bitplaneStrip.planes[3];

	const unsigned char allBits = BIT_0 | BIT_1 | BIT_2 | BIT_3;

	_asm{
		; Set write mode
			mov dx, 03CEh ; 6845 command register
			; Specify mode register
			mov al, 5
			; Write. Latched data will be used where the mask bits are 0.
			mov ah, 0
			; Send
			out dx, ax

		; Set write mode
			mov dx, 03CEh
			; Data Rotate register
			mov al, 3
			; No bit rotation, no AND, OR or XOR.
			mov ah, 0
			; Send
			out dx, ax

		; Set 6845 bit mask register
			; Specify bit mask register
			mov al, 8
			mov ah, mask
			; Send bit mask
			out dx, ax

		; Load existing pixels to latch, so masking works.
			mov ebx, stripAddress
			mov al, [ebx]

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
			mov ebx, stripAddress
			; Get the pixel value
			mov al, bitplane0
			; Write the pixel
			mov [ebx], al

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
			mov ebx, stripAddress
			; Get the pixel value
			mov al, bitplane1
			; Write the pixel
			mov [ebx], al

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
			mov ebx, stripAddress
			; Get the pixel value
			mov al, bitplane2
			; Write the pixel
			mov [ebx], al

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
			mov ebx, stripAddress
			; Get the pixel value
			mov al, bitplane3
			; Write the pixel
			mov [ebx], al

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


void copyStrip (unsigned short int index) {
	unsigned char* stripAddress = bufferBaseAddress + index;

	_asm{
		; Load the strip
			mov ebx, stripAddress
			; Load 6845 latch registers. It stays in the latch. The value read to the register is irrelevant.
			mov al, [ebx]
	}
}

void pasteStrip (unsigned short int index, unsigned char mask) {
	unsigned char *stripAddress = bufferBaseAddress + index;

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

		; Draw the strip
			mov ebx, stripAddress
			; Write the pixel. The value is irrelevant.
			mov [ebx], 0
	}
}


void setPalette (unsigned char palette[16]) {
	union REGS regs;
	int i;

	// Service 0. Set an individual palette register.
	regs.w.ax = 0x1000;

	for (i=0; i<16; ++i) {
		// BL holds the palette index to be set. (0-15)
		regs.h.bl = i;
		// BH holds the new color.
		regs.h.bh = palette[i];
		// Tell BIOS to set it.
		int386(BIOS_INTERRUPT_VIDEO, &regs, &regs);
	}
}


void setVirtualScreenWidth (unsigned short int numColumns) {

	// 3d4h index 13h (W):  CRTC: Offset register
	// bit 0-7  Number of bytes in a scanline / K. Where K is 2 for byte mode, 4 for
	//          word mode and 8 for Double Word mode.

	unsigned char bufferWidthInWords = numColumns/2;

	_asm{
		mov dx, 0x3d4
		mov al, 0x13
		mov ah, bufferWidthInWords
		; Send
		out dx, ax
	}
}


void setBufferOffset (unsigned short int offset, unsigned char restX) {

	unsigned char offsetHighBits = offset >> 8;
	unsigned char offsetLowBits = offset;

	unsigned char pelRegAddress = 0x13 | 0x20;

	// 3d4h index 0Ch (W):  CRTC: Start Address High Register
	// bit 0-7  Upper 8 bits of the start address of the display buffer

	// 3d4h index 0Dh (W):  CRTC: Start Address Low Register
	// bit 0-7  Lower 8 bits of the start address of the display buffer

	// 3C0h index 13h (W):  Attribute: Horizontal PEL Panning Register
	// bit 0-3  Indicates number of pixels to shift the display left

	_asm{
		mov dx, 0x3d4
		mov al, 0xc
		mov ah, offsetHighBits
		; Send
		out dx, ax
	}
	_asm{
		mov dx, 0x3d4
		mov al, 0xd
		mov ah, offsetLowBits
		; Send
		out dx, ax
	}
	_asm{
		mov dx, 0x3c0

		mov al, pelRegAddress
		; Send
		out dx, al

		mov al, restX
		; Send
		out dx, al
	}
}
