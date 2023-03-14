#include "text.h"
#include "font.h"
#include "myLib.h"

void drawChar(int row, int col, char ch, unsigned int color) {

	for (int r = 0; r < 8; r++) {
		for (int c = 0; c < 6; c++) {
			if (fontdata_6x8[OFFSET(r, c, 6) + ch * 48])
				setPixel(row + r, col + c, color);
			else
				setPixel(row + r, col + c, BLACK);
		}
	}
}

void drawString(int row, int col, char str[], unsigned short color) {
	for (int i = 0; str[i] != 0; i++) {
		drawChar(row, col + i * 6, str[i], color);
	}
}

/*void drawString(int row, int col, char *str, unsigned short color) {
        while (*str) {
                drawChar(row, col, *str++, color);
		col += 6;
        }
}*/
