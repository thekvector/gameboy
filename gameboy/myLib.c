#include "myLib.h"

volatile unsigned short *videoBuffer = (unsigned short *)0x6000000;
// setPixel -- set the pixel at (row, col) to color
	void setPixel(int row, int col, unsigned short color) {
		videoBuffer[OFFSET(row, col, 240)] = color;
	}
	void drawRect(int row, int col, int height, int width, unsigned short 
	color) {
		for(int r=0; r<height; r++) {
			/*
			for(int c = 0; c<width; c++) {
			setPixel(row+r, col+c, color);
			}
			*/
			DMA[3].src = &color;
			DMA[3].dst = &videoBuffer[OFFSET(row + r, col, 240)];
			DMA[3].cnt = width | DMA_ON | DMA_SOURCE_FIXED;
		}
	}
	
	void delay(int n) {
		volatile int x = 0;
		for(int i=0; i<n*8000; i++) {
			x++;
		}
	}

	void drawImage3(int r, int c, int width, int height, const u16* image) {
		for (int i = 0; i < height; i++) {
			DMA[3].src = image + OFFSET(i, 0, width);
			DMA[3].dst = videoBuffer + OFFSET(r + i, c, 240);
			DMA[3].cnt = DMA_ON | width | DMA_SOURCE_INCREMENT;
		}
	}

	void undrawImage3(int row, int col, int width, int height, const u16* image) {
		for (int i = row; i < (row + height); i++) {
			DMA[3].src = image + OFFSET(i, col, 240);
			DMA[3].dst = videoBuffer + OFFSET(i, col, 240);
			DMA[3].cnt = DMA_ON | DMA_SOURCE_INCREMENT | width;
		}
	}

	void waitForVblank() {
		while(SCANLINECOUNTER > 160)
			;
		while(SCANLINECOUNTER < 160)
			;
	}