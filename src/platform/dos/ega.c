#include "ega.h"
#include "bitplane_strip.h"

#include <conio.h>

#define BIOS_INTERRUPT_VIDEO 0X10 // The BIOS video interrupt number.
#define VIDEO_MODE_EGA_640_350_16 0x10 // EGA 640 x 350, 16 colors.

uint8_t *bufferBaseAddress = (uint8_t*) (0xa000 * 16);


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
	_asm {
		// service 0 - set current video mode
		mov AH, 0x0
		// requested mode
		mov AL, VIDEO_MODE_EGA_640_350_16
		int 0x10
	}
}


void drawPoint (uint16_t x, uint16_t y, uint8_t color) {
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


void drawStrip (uint16_t index, BitplaneStrip bitplaneStrip, uint8_t mask) {

	/*
	write modes:
		* 0. Write the latched pixels, combined with CPU data as per register 3. Default is unmodified CPU/latch depending on mask.
		* 1. Write the latched pixels. The byte written by the cpu are irrelevant. (Not sure how this is different from mode 0 with a zeroed out mask.)
		* 2. Write a single color to all 8 pixels.
	*/

	uint8_t *stripAddress = bufferBaseAddress + index;

	uint8_t bitplane0 = bitplaneStrip.planes[0];
	uint8_t bitplane1 = bitplaneStrip.planes[1];
	uint8_t bitplane2 = bitplaneStrip.planes[2];
	uint8_t bitplane3 = bitplaneStrip.planes[3];

	const uint8_t allBits = BIT_0 | BIT_1 | BIT_2 | BIT_3;

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

		; Write the stripAddress to the register, so dont overwrite it.
			mov ebx, stripAddress

		; Load existing pixels to latch, so masking works.
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


void drawStrips (uint16_t* beginIndex, uint16_t* endIndex, BitplaneStrip bitplaneStrip) {
	register uint16_t* iterator;

	drawStrip (*beginIndex, bitplaneStrip, 0xFF);
	copyStrip (*beginIndex);

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
			mov ah, 0xFF
			; Send bit mask
			out dx, ax
	}

	for (iterator=beginIndex; iterator!=endIndex; ++iterator) {
		uint8_t *stripAddress = bufferBaseAddress + *iterator;
		_asm{
			; Draw the strip
				mov ebx, stripAddress
				; Write the pixel. The value is irrelevant.
				mov [ebx], 0
		}
	}
}


void drawCustomStrips (PositionAndStrip *stripBatchBegin, PositionAndStrip *stripBatchEnd) {
	uint8_t planeIndex;
	register PositionAndStrip *iterator;

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
			mov dx, 03CEh
			; Specify bit mask register
			mov al, 8
			mov ah, 0xff
			; Send bit mask
			out dx, ax
	}

	for (planeIndex=0; planeIndex<4; ++planeIndex) {
		const uint8_t planeBit = BIT_0<<planeIndex;
		_asm{
			; Enable one plane at a time
				; 6845 command register
				mov dx, 3C4h
				; Specify sequencer register
				mov al, 2
				; only this plane
				mov ah, planeBit
				; Send
				out dx, ax
		}

		for (iterator=stripBatchBegin; iterator!=stripBatchEnd; ++iterator) {
			uint8_t *stripAddress = bufferBaseAddress + iterator->pos;
			uint8_t bitplane = iterator->strip.planes[planeIndex];

			_asm{
				; Write the stripAddress to the register, so dont overwrite it.
					mov ebx, stripAddress

				; Draw the strip
					; Get the pixel value
					mov al, bitplane
					; Write the pixel
					mov [ebx], al
			}
		}
	}

	{
		const uint8_t allBits = BIT_0 | BIT_1 | BIT_2 | BIT_3;

		_asm{
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
}


void copyStrip (uint16_t index) {
	uint8_t* stripAddress = bufferBaseAddress + index;

	_asm{
		; Load the strip
			mov ebx, stripAddress
			; Load 6845 latch registers. It stays in the latch. The value read to the register is irrelevant.
			mov al, [ebx]
	}
}

void pasteStrip (uint16_t index, uint8_t mask) {
	uint8_t *stripAddress = bufferBaseAddress + index;

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


void setPalette (uint8_t palette[16]) {
	uint8_t i;
	for (i=0; i<16; ++i) {
		uint8_t  color = palette[i];
		_asm {
			// Service 0. Set an individual palette register.
			mov AX, 0x1000
			// BL holds the palette index to be set. (0-15)
			mov BL, i
			// BH holds the new color.
			mov BH, color
			// Tell BIOS to set it.
			int BIOS_INTERRUPT_VIDEO
		}
	}
}


void setVirtualScreenWidth (uint16_t numColumns) {

	// 3d4h index 13h (W):  CRTC: Offset register
	// bit 0-7  Number of bytes in a scanline / K. Where K is 2 for byte mode, 4 for
	//          word mode and 8 for Double Word mode.

	uint8_t bufferWidthInWords = numColumns/2;

	_asm{
		mov dx, 0x3d4
		mov al, 0x13
		mov ah, bufferWidthInWords
		; Send
		out dx, ax
	}
}


void setBufferOffset (uint16_t offset, uint8_t restX) {

	uint8_t offsetHighBits = offset >> 8;
	uint8_t offsetLowBits = offset;

	uint8_t pelRegAddress = 0x13 | 0x20;

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
