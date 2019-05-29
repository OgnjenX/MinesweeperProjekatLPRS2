/*
 * Copyright (c) 2009-2012 Xilinx, Inc.  All rights reserved.
 *
 * Xilinx, Inc.
 * XILINX IS PROVIDING THIS DESIGN, CODE, OR INFORMATION "AS IS" AS A
 * COURTESY TO YOU.  BY PROVIDING THIS DESIGN, CODE, OR INFORMATION AS
 * ONE POSSIBLE   IMPLEMENTATION OF THIS FEATURE, APPLICATION OR
 * STANDARD, XILINX IS MAKING NO REPRESENTATION THAT THIS IMPLEMENTATION
 * IS FREE FROM ANY CLAIMS OF INFRINGEMENT, AND YOU ARE RESPONSIBLE
 * FOR OBTAINING ANY RIGHTS YOU MAY REQUIRE FOR YOUR IMPLEMENTATION.
 * XILINX EXPRESSLY DISCLAIMS ANY WARRANTY WHATSOEVER WITH RESPECT TO
 * THE ADEQUACY OF THE IMPLEMENTATION, INCLUDING BUT NOT LIMITED TO
 * ANY WARRANTIES OR REPRESENTATIONS THAT THIS IMPLEMENTATION IS FREE
 * FROM CLAIMS OF INFRINGEMENT, IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 */

/*
 * helloworld.c: simple test application
 *
 * This application configures UART 16550 to baud rate 9600.
 * PS7 UART (Zynq) is not initialized by this application, since
 * bootrom/bsp configures it to baud rate 115200
 *
 * ------------------------------------------------
 * | UART TYPE   BAUD RATE                        |
 * ------------------------------------------------
 *   uartns550   9600
 *   uartlite    Configurable only in HW design
 *   ps7_uart    115200 (configured by bootrom/bsp)
 *
 *
 *
 */

#include <stdio.h>
#include "platform.h"
#include "xparameters.h"
#include "xio.h"
#include "xil_exception.h"
#include "vga_periph_mem.h"
#include "minesweeper_sprites.h"
#include <stdlib.h>     /* srand, rand */
#include <time.h>
#define SIZE 100
#define UP 0b01000000
#define DOWN 0b00000100
#define LEFT 0b00100000
#define RIGHT 0b00001000
#define CENTER 0b00010000
#define SW0 0b00000001
#define SW1 0b00000010
#define BLANK '0'
#define PLAYER '1'
#define BOT '2'
#define PLAYER_TRACE '3'
#define BOTS_TRACE '4'

int endOfGame;
int inc1;
int inc2;
int i, x, y, ii, oi, R, G, B, RGB, kolona, red, RGBgray,r,c;


char BlankMap[40][30];

typedef enum {
		LEVO,
		DESNO,
		GORE,
		DOLE
	}PRAVAC;

PRAVAC movePlayer(int* proslo_stanje,int *trenutno_stanje);
PRAVAC moveBot(int* proslo_stanje,int *trenutno_stanje );

/*


//end of game
void printOutEndOfGame(char blankTable[SIZE][SIZE], char solvedMap[SIZE][SIZE]) {
	int i, j, ii, jj;
	for (i = 0; i < SIZE; i++) {
		for (j = 0; j < SIZE; j++) {
			ii = (i * 16) + 80;
			jj = (j * 16) + 80;
			if (blankTable[i][j] == FLAG) {
				if (solvedMap[i][j] != BOMB) {
					drawMap(16, 16, ii, jj, 16, 16);
				}
			} else if (blankTable[i][j] != FLAG && solvedMap[i][j] == BOMB) {
				drawMap(0, 16, ii, jj, 16, 16);
			}
		}
	}
}

//when the blank field is pressed, open all blank fields around it

void traceP(int x, int y, char resultTable[SIZE][SIZE],
		char indicationMap[SIZE][SIZE]) {
	int i, j;

	indicationMap[x][y] = 'x';

	if (resultTable[x][y] == BLANK) {
		for (i = x - 1; i <= x + 1; i++) {
			for (j = y - 1; j <= y + 1; j++) {
				if (i >= 0 && j >= 0 && i < 9 && j < 9 && !(x == i && y == j)) {
					if (indicationMap[i][j] == BLANK) {
						traceP(i, j, resultTable, indicationMap);
					}
				}

			}
		}
	}
}

//function for opening selected field

void openField(int x, int y, char map[9][9]) {
	int i, j;
	int x1, y1;
	x1 = (x - 80) / 16;
	y1 = (y - 80) / 16;

	switch (map[x1][y1]) {
	case NUM1:
		drawMap(16, 0, x - 1, y - 1, 16, 16);
		if (map != blankMap)
			blankMap[x1][y1] = NUM1;
		break;

	case NUM2:
		drawMap(32, 0, x - 1, y - 1, 16, 16);
		if (map != blankMap)
			blankMap[x1][y1] = NUM2;
		break;

	case NUM3:
		drawMap(48, 0, x - 1, y - 1, 16, 16);
		if (map != blankMap)
			blankMap[x1][y1] = NUM3;
		break;

	case BLANK:
		drawMap(0, 0, x - 1, y - 1, 16, 16);
		if (map != blankMap)
			blankMap[x1][y1] = BLANK;
		traceP(x1, y1, solvedMap, indicationMap);
		for (i = 0; i < 9; i++) {
			for (j = 0; j < 9; j++) {
				xil_printf("%c", indicationMap[i][j]);
			}
			xil_printf("\n");
		}
		break;

	case NUM4:
		drawMap(64, 0, x - 1, y - 1, 16, 16);
		if (map != blankMap)
			blankMap[x1][y1] = NUM4;
		break;

	case BOMB:
		if (map != blankMap)
			blankMap[x1][y1] = BOMB;
		endOfGame = 1;
		printOutEndOfGame(blankMap, solvedMap);
		drawMap(32, 16, x - 1, y - 1, 16, 16);
		drawMap(77, 54, 120, 54, 27, 26);
		break;
	case '@':
		drawMap(80, 16, x - 1, y - 1, 16, 16);
		if (map != blankMap)
			blankMap[x1][y1] = BEG;
		break;

	case '#':
		drawMap(64, 16, x - 1, y - 1, 16, 16);
		if (map != blankMap)
			blankMap[x1][y1] = FLAG;
		break;
	}
}





//function that generates random game map
void makeTable(char temp[9][9]) {
	int numOfMines = NUMOFMINES, row, column, i, j, m, surroundingMines = 0;
	char table[9][9];

	srand(randomCounter);

	//popunjava matricu nulama
	for (i = 0; i < 9; i++) {
		for (j = 0; j < 9; j++) {
			table[i][j] = BLANK;
		}
	}

	//postavlja random mine
	while (numOfMines > 0) {
		row = rand() % 9;
		column = rand() % 9;
		if (table[row][column] == BLANK) {
			table[row][column] = BOMB;
			numOfMines--;
		}

	}

	//proverava poziciju mina i ispisuje brojeve na odg mesta
	for (i = 0; i < 9; i++) {
		for (j = 0; j < 9; j++) {
			surroundingMines = 0;
			if (table[i][j] != BOMB) {
				if (i > 0 && j > 0) {
					if (table[i - 1][j - 1] == BOMB)
						surroundingMines++;
				}
				if (j > 0) {
					if (table[i][j - 1] == BOMB)
						surroundingMines++;
				}
				if (i < 9 - 1 && j > 0) {
					if (table[i + 1][j - 1] == BOMB)
						surroundingMines++;
				}
				if (i > 0) {
					if (table[i - 1][j] == BOMB)
						surroundingMines++;
				}
				if (i < 9 - 1) {
					if (table[i + 1][j] == BOMB)
						surroundingMines++;
				}
				if (i > 0 && j < 9 - 1) {
					if (table[i - 1][j + 1] == BOMB)
						surroundingMines++;
				}
				if (j < 9 - 1) {
					if (table[i][j + 1] == BOMB)
						surroundingMines++;
				}
				if (i < 9 - 1 && j < 9 - 1) {
					if (table[i + 1][j + 1] == BOMB)
						surroundingMines++;
				}
				table[i][j] = surroundingMines + '0';
			}
		}

	}

	//for testing

	for (i = 0; i < 9; i++) {
		for (j = 0; j < 9; j++) {
			xil_printf("%c", table[i][j]);
		}
		xil_printf("\n");
	}

	for (i = 0; i < 9; i++) {
		for (j = 0; j < 9; j++) {
			temp[i][j] = table[j][i];

		}
	}

	xil_printf("\n");

	for (i = 0; i < 9; i++) {
		for (j = 0; j < 9; j++) {
			xil_printf("%c", temp[j][i]);
		}
		xil_printf("\n");
	}

}



//extracting pixel data from a picture for printing out on the display





void drawMap(int in_x, int in_y, int out_x, int out_y, int width, int height) {
	int ox, oy, oi, iy, ix, ii;
	for (y = 0; y < height; y++) {
		for (x = 0; x < width; x++) {
			ox = out_x + x;
			oy = out_y + y;
			oi = oy * 320 + ox;
			ix = in_x + x;
			iy = in_y + y;
			ii = iy * minesweeper_sprites.width + ix;
			R = minesweeper_sprites.pixel_data[ii
					* minesweeper_sprites.bytes_per_pixel] >> 5;
			G = minesweeper_sprites.pixel_data[ii
					* minesweeper_sprites.bytes_per_pixel + 1] >> 5;
			B = minesweeper_sprites.pixel_data[ii
					* minesweeper_sprites.bytes_per_pixel + 2] >> 5;
			R <<= 6;
			G <<= 3;
			RGB = R | G | B;

			VGA_PERIPH_MEM_mWriteMemory(
					XPAR_VGA_PERIPH_MEM_0_S_AXI_MEM0_BASEADDR + GRAPHICS_MEM_OFF
							+ oi * 4, RGB);
		}
	}

}



//drawing cursor for indicating position
void drawingCursor(int startX, int startY, int endX, int endY) {

	for (x = startX; x < endX; x++) {
		for (y = startY; y < startY + 2; y++) {
			i = y * 320 + x;
			VGA_PERIPH_MEM_mWriteMemory(
					XPAR_VGA_PERIPH_MEM_0_S_AXI_MEM0_BASEADDR + GRAPHICS_MEM_OFF
							+ i * 4, 0x000000);
		}
	}

	for (x = startX; x < endX; x++) {
		for (y = endY - 2; y < endY; y++) {
			i = y * 320 + x;
			VGA_PERIPH_MEM_mWriteMemory(
					XPAR_VGA_PERIPH_MEM_0_S_AXI_MEM0_BASEADDR + GRAPHICS_MEM_OFF
							+ i * 4, 0x000000);
		}
	}

	for (x = startX; x < startX + 2; x++) {
		for (y = startY; y < endY; y++) {
			i = y * 320 + x;
			VGA_PERIPH_MEM_mWriteMemory(
					XPAR_VGA_PERIPH_MEM_0_S_AXI_MEM0_BASEADDR + GRAPHICS_MEM_OFF
							+ i * 4, 0x000000);
		}
	}

	for (x = endX - 2; x < endX; x++) {
		for (y = startY; y < endY; y++) {
			i = y * 320 + x;
			VGA_PERIPH_MEM_mWriteMemory(
					XPAR_VGA_PERIPH_MEM_0_S_AXI_MEM0_BASEADDR + GRAPHICS_MEM_OFF
							+ i * 4, 0x000000);
		}
	}

}

//function that controls switches and buttons





void move() {
	int startX = 145, startY = 201, endX = 96, endY = 96;
	int oldStartX, oldStartY, oldEndX, oldEndY;
	int x, y, ic, ib, i, j;
	int prethodnoStanje;
	typedef enum {
		NOTHING_PRESSED, SOMETHING_PRESSED
	} btn_state_t;
	btn_state_t btn_state = NOTHING_PRESSED;

	makeTable(solvedMap);
	drawingCursor(startX, startY, endX, endY);

	while (endOfGame != 1) {

		if (btn_state == NOTHING_PRESSED) {
			btn_state = SOMETHING_PRESSED;
			if ((Xil_In32(XPAR_MY_PERIPHERAL_0_BASEADDR) & DOWN) == 0) {
				if (endY < 224) {
					oldStartY = startY;
					oldEndY = endY;
					startY += 16;
					endY += 16;
					drawingCursor(startX, startY, endX, endY);
					openField(startX, oldStartY, blankMap);
				}

			}

			else if ((Xil_In32(XPAR_MY_PERIPHERAL_0_BASEADDR) & RIGHT) == 0) {
				randomCounter++;
				if (endX < 224) {
					oldStartX = startX;
					startX += 16;
					endX += 16;
					drawingCursor(startX, startY, endX, endY);
					openField(oldStartX, startY, blankMap);

				}
			} else if ((Xil_In32(XPAR_MY_PERIPHERAL_0_BASEADDR) & LEFT) == 0) {
				if (startX > 81) {
					oldStartX = startX;
					startX -= 16;
					endX -= 16;
					drawingCursor(startX, startY, endX, endY);
					openField(oldStartX, startY, blankMap);
				}

			} else if ((Xil_In32(XPAR_MY_PERIPHERAL_0_BASEADDR) & UP) == 0) {
				if (startY > 81) {
					oldStartY = startY;
					startY -= 16;
					endY -= 16;
					drawingCursor(startX, startY, endX, endY);
					openField(startX, oldStartY, blankMap);
				}

			} else if ((Xil_In32(XPAR_MY_PERIPHERAL_0_BASEADDR) & CENTER)
					== 0) {
				int m = (startX - 80) / 16;
				int n = (startY - 80) / 16;
				firstTimeCenter++;
				if (firstTimeCenter == 1) {
					randomCounter++;
					while (solvedMap[m][n] == BOMB)
						makeTable(solvedMap);
				}
				openField(startX, startY, solvedMap);
				int ii = 0, jj = 0;

				for (i = 0; i < SIZE; i++) {
					for (j = 0; j < SIZE; j++) {
						if (indicationMap[i][j] == 'x') {
							ii = (i * 16) + 80;
							jj = (j * 16) + 80;

							if (solvedMap[i][j] == BLANK) {
								drawMap(0, 0, ii, jj, 16, 16);
								blankMap[i][j] = BLANK;
							}
							if (solvedMap[i][j] == NUM2) {
								drawMap(32, 0, ii, jj, 16, 16);
								blankMap[i][j] = NUM2;
							}
							if (solvedMap[i][j] == NUM1) {
								drawMap(16, 0, ii, jj, 16, 16);
								blankMap[i][j] = NUM1;
							}
							if (solvedMap[i][j] == NUM3) {
								drawMap(48, 0, ii, jj, 16, 16);
								blankMap[i][j] = NUM3;
							}
							if (solvedMap[i][j] == NUM4) {
								drawMap(64, 0, ii, jj, 16, 16);
								blankMap[i][j] = NUM4;
							}
						}
					}
				}

			} else if ((Xil_In32(XPAR_MY_PERIPHERAL_0_BASEADDR) & SW0) != 0) { //flag

				if (numOfFlags > 0 && numOfFlags <= NUMOFMINES) {
					int x = (startX - 80) / 16;
					int y = (startY - 80) / 16;
					if (blankMap[x][y] != FLAG && blankMap[x][y] == BEG) {
						drawMap(64, 16, startX - 1, startY - 1, 16, 16);

						blankMap[x][y] = FLAG;

						numOfFlags--;
						//checks if the flag is in the right place
						if (solvedMap[x][y] == BOMB) {
							flagTrue++;
							if (flagTrue == NUMOFMINES) {

								endOfGame = 1;
								drawMap(103, 54, 120, 54, 27, 26);
							}
						}

					}
					//prints out flag counter
					switch (numOfFlags) {
					case 9:
						drawMap(116, 32, 168, 54, 13, 23);
						break;
					case 8:
						drawMap(103, 32, 168, 54, 13, 23);
						break;
					case 7:
						drawMap(90, 32, 168, 54, 13, 23);
						break;
					case 6:
						drawMap(77, 32, 168, 54, 13, 23);
						break;
					case 5:
						drawMap(64, 32, 168, 54, 14, 23);
						break;
					case 4:
						drawMap(51, 32, 168, 54, 13, 23);
						break;
					case 3:
						drawMap(38, 32, 168, 54, 13, 23);
						break;
					case 2:
						drawMap(25, 32, 168, 54, 13, 23);
						break;
					case 1:
						drawMap(13, 32, 168, 54, 13, 23);
						break;
					case 0:
						drawMap(0, 32, 168, 54, 13, 23);
						break;

					}

				}
			} else if ((Xil_In32(XPAR_MY_PERIPHERAL_0_BASEADDR) & SW1) != 0) {
				if (numOfFlags < NUMOFMINES) {

					int x = (startX - 80) / 16;
					int y = (startY - 80) / 16;
					if (blankMap[x][y] == FLAG) {
						drawMap(80, 16, startX - 1, startY - 1, 16, 16);

						blankMap[x][y] = BEG;

						numOfFlags++;

						if (solvedMap[x][y] == BOMB) {
							flagTrue--;
						}

						switch (numOfFlags) {
						case 9:
							drawMap(116, 32, 168, 54, 13, 23);
							break;
						case 8:
							drawMap(103, 32, 168, 54, 13, 23);
							break;
						case 7:
							drawMap(90, 32, 168, 54, 13, 23);
							break;
						case 6:
							drawMap(77, 32, 168, 54, 13, 23);
							break;
						case 5:
							drawMap(64, 32, 168, 54, 13, 23);
							break;
						case 4:
							drawMap(51, 32, 168, 54, 13, 23);
							break;
						case 3:
							drawMap(38, 32, 168, 54, 13, 23);
							break;
						case 2:
							drawMap(25, 32, 168, 54, 13, 23);
							break;
						case 1:
							drawMap(13, 32, 168, 54, 13, 23);
							break;
						case 0:
							drawMap(0, 32, 168, 54, 13, 23);
							break;
						}
					}
				}

			} else {
				btn_state = NOTHING_PRESSED;
			}
		} else { // SOMETHING_PRESSED
			if ((Xil_In32(XPAR_MY_PERIPHERAL_0_BASEADDR) & DOWN) == 0) {
			} else if ((Xil_In32(XPAR_MY_PERIPHERAL_0_BASEADDR) & RIGHT) == 0) {
			} else if ((Xil_In32(XPAR_MY_PERIPHERAL_0_BASEADDR) & LEFT) == 0) {
			} else if ((Xil_In32(XPAR_MY_PERIPHERAL_0_BASEADDR) & UP) == 0) {
			} else if ((Xil_In32(XPAR_MY_PERIPHERAL_0_BASEADDR) & CENTER)
					== 0) {
			} else if ((Xil_In32(XPAR_MY_PERIPHERAL_0_BASEADDR) & SW0) != 0) {
			} else if ((Xil_In32(XPAR_MY_PERIPHERAL_0_BASEADDR) & SW1) != 0) {
			} else {
				btn_state = NOTHING_PRESSED;
			}
		}

	}

} */



int main() {

	inc1 = 0;
	inc2 = 0;


	init_platform();


	VGA_PERIPH_MEM_mWriteMemory(
			XPAR_VGA_PERIPH_MEM_0_S_AXI_MEM0_BASEADDR + 0x00, 0x0); // direct mode   0
	VGA_PERIPH_MEM_mWriteMemory(
			XPAR_VGA_PERIPH_MEM_0_S_AXI_MEM0_BASEADDR + 0x04, 0x3); // display_mode  1
	VGA_PERIPH_MEM_mWriteMemory(
			XPAR_VGA_PERIPH_MEM_0_S_AXI_MEM0_BASEADDR + 0x08, 0x0); // show frame      2
	VGA_PERIPH_MEM_mWriteMemory(
			XPAR_VGA_PERIPH_MEM_0_S_AXI_MEM0_BASEADDR + 0x0C, 0xff); // font size       3
	VGA_PERIPH_MEM_mWriteMemory(
			XPAR_VGA_PERIPH_MEM_0_S_AXI_MEM0_BASEADDR + 0x10, 0x00FF00); // foreground 4
	VGA_PERIPH_MEM_mWriteMemory(
			XPAR_VGA_PERIPH_MEM_0_S_AXI_MEM0_BASEADDR + 0x14, 0x000000); // background color 5
	VGA_PERIPH_MEM_mWriteMemory(
			XPAR_VGA_PERIPH_MEM_0_S_AXI_MEM0_BASEADDR + 0x18, 0xFF0000); // frame color      6
	VGA_PERIPH_MEM_mWriteMemory(
			XPAR_VGA_PERIPH_MEM_0_S_AXI_MEM0_BASEADDR + 0x20, 1);

	//black background
	for (x = 0; x < 320; x++) {
		for (y = 0; y < 240; y++) {
			i = y * 320 + x;
			VGA_PERIPH_MEM_mWriteMemory(
					XPAR_VGA_PERIPH_MEM_0_S_AXI_MEM0_BASEADDR + GRAPHICS_MEM_OFF
							+ i * 4, 0x000000);
		}
	}

	for (x = 0; x < 320; x++) {
			for (y = 0; y < 240; y++) {
				if(x==0 || x==319 || y==0 || y==239 )
				i = y * 320 + x;
				VGA_PERIPH_MEM_mWriteMemory(
						XPAR_VGA_PERIPH_MEM_0_S_AXI_MEM0_BASEADDR + GRAPHICS_MEM_OFF
								+ i * 4, 0xFFFFFF);
			}
		}



	for (kolona = 0; kolona < 40; kolona++) {
			for (red = 0; red < 30; red++) {
				if(kolona == 20 && red == 27 ) {
				drawMap(8,0  , red * 8,  kolona * 8, 8, 8);
				}
			}
	}

	for (kolona = 0; kolona < 40; kolona++) {
				for (red = 0; red < 30; red++) {
					if(kolona == 20 && red == 3 ) {
					drawMap(0,0  , red * 8,  kolona * 8, 8, 8);
					}
				}
		}

	movePlayerAndBot();

	cleanup_platform();

	return 0;
}

void makeTable(char temp [40][30]) {
	int i,j;
	char table[40][30];

	//popunjava matricu nulama
	for (i = 0; i < 40; i++) {
		for (j = 0; j < 30; j++) {
			table[i][j] = BLANK;
		}
	}

	table[20][27] = PLAYER;
	table[20][3]=BOT;

}

//crtanje spritova
void drawMap(int in_x, int in_y, int out_x, int out_y, int width, int height) {
	int ox, oy, oi, iy, ix, ii;
	for (y = 0; y < height; y++) {
		for (x = 0; x < width; x++) {
			ox = out_x + x;
			oy = out_y + y;
			oi = oy * 320 + ox;
			ix = in_x + x;
			iy = in_y + y;
			ii = iy * minesweeper_sprites.width + ix;
			R = minesweeper_sprites.pixel_data[ii
					* minesweeper_sprites.bytes_per_pixel] >> 5;
			G = minesweeper_sprites.pixel_data[ii
					* minesweeper_sprites.bytes_per_pixel + 1] >> 5;
			B = minesweeper_sprites.pixel_data[ii
					* minesweeper_sprites.bytes_per_pixel + 2] >> 5;
			R <<= 6;
			G <<= 3;
			RGB = R | G | B;

			VGA_PERIPH_MEM_mWriteMemory(
					XPAR_VGA_PERIPH_MEM_0_S_AXI_MEM0_BASEADDR + GRAPHICS_MEM_OFF
							+ oi * 4, RGB);
		}
	}

}

void traceP(int in_x, int in_y, int out_x, int out_y, int width, int height) {
	int ox, oy, oi, iy, ix, ii;
	for (y = 0; y < height; y++) {
		for (x = 0; x < width; x++) {
			ox = out_x + x;
			oy = out_y + y;
			oi = oy * 320 + ox;


			VGA_PERIPH_MEM_mWriteMemory(
					XPAR_VGA_PERIPH_MEM_0_S_AXI_MEM0_BASEADDR + GRAPHICS_MEM_OFF
							+ oi * 4, 0x0000FF);
		}
	}

}

void traceB(int in_x, int in_y, int out_x, int out_y, int width, int height) {
	int ox, oy, oi, iy, ix, ii;
	for (y = 0; y < height; y++) {
		for (x = 0; x < width; x++) {
			ox = out_x + x;
			oy = out_y + y;
			oi = oy * 320 + ox;


			VGA_PERIPH_MEM_mWriteMemory(
					XPAR_VGA_PERIPH_MEM_0_S_AXI_MEM0_BASEADDR + GRAPHICS_MEM_OFF
							+ oi * 4, 0x00FF00);
		}
	}

}

void movePlayerAndBot(char table[40][30]) {
	int rowP=20, columnP=27;
	int rowB=20, columnB=3;
	int proslo_stanje = 4;
	int trenutno_stanje = 4;
	int proslo_stanje_b = 3;
	int trenutno_stanje_b = 3;
	int brojac = 0;
	int j = 0;
	int s= 0;
	int row,column;
	PRAVAC p;
	PRAVAC b;

	makeTable(BlankMap);


	while(1){
		while(j<1000000) {
			if(j==1000){
				b = moveBot(&proslo_stanje_b, &trenutno_stanje_b);
			}
			j++;
			}
		j=0;


		switch(b){
			case DOLE:
				columnB++;
				drawMap(0,0,rowB*8,columnB*8,8,8);
				traceB(0,0,rowB*8,(columnB-1)*8,8,8);
				break;
			case GORE:
				columnB--;
				drawMap(0,0,rowB*8,columnB*8,8,8);
				traceB(0,0,rowB*8,(columnB+1)*8,8,8);
				break;
			case LEVO:
				rowB--;
				drawMap(0,0,rowB*8,columnB*8,8,8);
				traceB(0,0,(rowB+1)*8,columnB*8,8,8);
				break;
			case DESNO:
				rowB++;
				drawMap(0,0,rowB*8,columnB*8,8,8);
				traceB(0,0,(rowB-1)*8,columnB*8,8,8);
				break;
				}

		BlankMap[columnB][rowB] = BOT;

			p = movePlayer(&proslo_stanje,&trenutno_stanje);
		switch(p){
			case DOLE:
				columnP++;
				drawMap(8,0,rowP*8,columnP*8,8,8);
				traceP(8,0,rowP*8,(columnP-1)*8,8,8);
				break;
			case GORE:
				columnP--;
				drawMap(8,0,rowP*8,columnP*8,8,8);
				traceP(8,0,rowP*8,(columnP+1)*8,8,8);
				break;
			case LEVO:
				rowP--;
				drawMap(8,0,rowP*8,columnP*8,8,8);
				traceP(8,0,(rowP+1)*8,columnP*8,8,8);
				break;
			case DESNO:
				rowP++;
				drawMap(8,0,rowP*8,columnP*8,8,8);
				traceP(8,0,(rowP-1)*8,columnP*8,8,8);
				break;

	}
		BlankMap[columnP][rowP] = PLAYER;


	for(row = 0; row < 30; row++) {
		for(column=0; column < 40; column++) {
			if(BlankMap[column][row] == PLAYER) {
				if(row != rowP || column !=columnP) {
					BlankMap[column][row] = PLAYER_TRACE;
				}
			}
			if(BlankMap[column][row] == BOT) {
				if(row != rowB || column !=columnB) {
					BlankMap[column][row] = BOTS_TRACE;
				}
			}
		}
	}


}
}

PRAVAC moveBot( int* proslo_stanje,int *trenutno_stanje) {

	int *t = trenutno_stanje;
	int *p = proslo_stanje;

	PRAVAC pravac;
	 *t = rand()%4;


	if(*t == *p){
		switch(*p) {
			case 1:
				return DESNO;
			case 2:
				return LEVO;
			case 3:
				return DOLE;
			case 4:
				return GORE;
			}
		} else{
			if(*p != 2 && *t == 1) {
				pravac = DESNO;
				*p= 1;
			} else if(*p != 1 && *t == 2) {
				pravac = LEVO;
				*p= 2;

			} else if(*t == 3 && *p != 4) {
				pravac = DOLE;
				*p = 3;

			} else if(*t == 4 && *p != 3) {
				pravac = GORE;
				*p = 4;
			}
		}
	return pravac;


}

PRAVAC movePlayer(int* proslo_stanje,int *trenutno_stanje) {

	/*
	 * RIGHT - 1
	 * LEFT - 2
	 * DOWN - 3
	 * UP - 4
	 */
	int *t = trenutno_stanje;
	int *p = proslo_stanje;


	PRAVAC pravac;



		if((Xil_In32(XPAR_MY_PERIPHERAL_0_BASEADDR) & RIGHT) == 0) {
				*t = 1;

			} else if((Xil_In32(XPAR_MY_PERIPHERAL_0_BASEADDR) & LEFT) == 0) {
				*t = 2;

			} else if((Xil_In32(XPAR_MY_PERIPHERAL_0_BASEADDR) & DOWN) == 0) {
				*t = 3;

			} else if((Xil_In32(XPAR_MY_PERIPHERAL_0_BASEADDR) & UP) == 0) {
				*t = 4;
			}
		if(*t == *p){
			switch(*p) {
			case 1:
				return DESNO;
			case 2:
				return LEVO;
			case 3:
				return DOLE;
			case 4:
				return GORE;
			}
		} else{
			if(*p != 2 && *t == 1) {
						pravac = DESNO;
						*p= 1;
					} else if(*p != 1 && *t == 2) {
						pravac = LEVO;
						*p= 2;

					} else if(*t == 3 && *p != 4) {
						pravac = DOLE;
						*p = 3;

					} else if(*t == 4 && *p != 3) {
						pravac = GORE;
						*p = 4;
					}


		}
		return pravac;
}



