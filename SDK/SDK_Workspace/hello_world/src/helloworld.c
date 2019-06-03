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
#include <stdbool.h>
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
int endGame(int column, int row);
void printMargines();
PRAVAC botSurvivalMode();
void printEndGameYellowWins();
void printEndGameBlueWins();
void makeTable(char table [40][30]);
void movePlayerAndBot();
PRAVAC botSurvivalMode(int* proslo_stanje);
void drawMap(int in_x, int in_y, int out_x, int out_y, int width, int height);




int main()
{

	inc1 = 0;
	inc2 = 0;


	init_platform();

	srand(time(0));

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

	printMargines();

	movePlayerAndBot();

	cleanup_platform();

	return 0;
}

void makeTable(char table [40][30])
{
	int i,j;


	//popunjava matricu nulama
	for (i = 0; i < 40; i++) {
		for (j = 0; j < 30; j++) {
			table[i][j] = BLANK;
		}
	}

	table[20][27] = PLAYER;
	table[20][3]=BOT;


}

void printMargines()
{
	for (x = 0; x < 320; x++) {
		for (y = 0; y < 240; y++) {
			if(x==0 || x==319 || y==0 || y==239 )
				i = y * 320 + x;
				VGA_PERIPH_MEM_mWriteMemory(
				XPAR_VGA_PERIPH_MEM_0_S_AXI_MEM0_BASEADDR + GRAPHICS_MEM_OFF
				+ i * 4, 0xFFFFFF);
		}
	}
}

//crtanje spritova
void drawMap(int in_x, int in_y, int out_x, int out_y, int width, int height)
{
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


void BlackClear(int in_x, int in_y, int out_x, int out_y, int width, int height)
{
	int ox, oy, oi;
	for (y = 0; y < height; y++) {
		for (x = 0; x < width; x++) {
			ox = out_x + x;
			oy = out_y + y;
			oi = oy * 320 + ox;


			VGA_PERIPH_MEM_mWriteMemory(
					XPAR_VGA_PERIPH_MEM_0_S_AXI_MEM0_BASEADDR + GRAPHICS_MEM_OFF
							+ oi * 4, 0x000000);

		}
	}

}

void movePlayerAndBot()
{
	int columnP=20, rowP=27;
	int columnB=20, rowB=3;
	int proslo_stanje = 4;
	int trenutno_stanje = 4;
	int proslo_stanje_b = 3;
	int trenutno_stanje_b = 3;
	int j = 0;
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

		if(BlankMap[columnB+1][rowB] == PLAYER_TRACE || BlankMap[columnB][rowB+1] == PLAYER_TRACE)
		{
			b = botSurvivalMode(&proslo_stanje_b);
		}

		if(columnB > 38 || rowB > 28 || columnB < 1 || rowB < 1)
		{
			b = botSurvivalMode(&proslo_stanje_b);
		}



		switch(b){
			case DOLE:
				rowB++;
				drawMap(48,0,columnB*8,rowB*8,8,8);
				drawMap(72,0,columnB*8,(rowB-1)*8,8,8);
				break;
			case GORE:
				rowB--;
				drawMap(32,0,columnB*8,rowB*8,8,8);
				drawMap(72,0,columnB*8,(rowB+1)*8,8,8);
				break;
			case LEVO:
				columnB--;
				drawMap(56,0,columnB*8,rowB*8,8,8);
				drawMap(64,0,(columnB+1)*8,rowB*8,8,8);
				break;
			case DESNO:
				columnB++;
				drawMap(40,0,columnB*8,rowB*8,8,8);
				drawMap(64,0,(columnB-1)*8,rowB*8,8,8);
				break;
				}

		if(endGame(columnB,rowB) == 1)
		{
			drawMap(96,0,columnB*8,rowB*8,8,8);
			printEndGameBlueWins();
			break;
		} else if (endGame(columnB,rowB) == 2) {
			drawMap(96,0,(columnB+1)*8,rowB*8,8,8);
			BlackClear(16,0,columnB*8,rowB*8,8,8);
			printMargines();
			printEndGameBlueWins();
			break;
		} else if (endGame(columnB,rowB) == 3) {
			drawMap(96,0,columnB*8,(rowB+1)*8,8,8);
			printEndGameBlueWins();
			break;
		} else if (endGame(columnB,rowB) == 4) {
			drawMap(96,0,(columnB-1)*8,rowB*8,8,8);
			BlackClear(16,0,columnB*8,rowB*8,8,8);
			printMargines();
			printEndGameBlueWins();
			break;
		} else if (endGame(columnB,rowB) == 5) {
			drawMap(96,0,columnB*8,(rowB-1)*8,8,8);
			printEndGameBlueWins();
			break;
		}

		BlankMap[columnB][rowB] = BOT;

		switch(b){
			case DOLE:
				rowB++;
				drawMap(48,0,columnB*8,rowB*8,8,8);
				drawMap(72,0,columnB*8,(rowB-1)*8,8,8);
				break;
			case GORE:
				rowB--;
				drawMap(32,0,columnB*8,rowB*8,8,8);
				drawMap(72,0,columnB*8,(rowB+1)*8,8,8);
				break;
			case LEVO:
				columnB--;
				drawMap(56,0,columnB*8,rowB*8,8,8);
				drawMap(64,0,(columnB+1)*8,rowB*8,8,8);
				break;
			case DESNO:
				columnB++;
				drawMap(40,0,columnB*8,rowB*8,8,8);
				drawMap(64,0,(columnB-1)*8,rowB*8,8,8);
				break;
				}

		if(endGame(columnB,rowB) == 1)
		{
			drawMap(96,0,columnB*8,rowB*8,8,8);
			printEndGameBlueWins();
			break;
		} else if (endGame(columnB,rowB) == 2) {
			drawMap(96,0,(columnB+1)*8,rowB*8,8,8);
			BlackClear(16,0,columnB*8,rowB*8,8,8);
			printMargines();
			printEndGameBlueWins();
			break;
		} else if (endGame(columnB,rowB) == 3) {
			drawMap(96,0,columnB*8,(rowB+1)*8,8,8);
			printEndGameBlueWins();
			break;
		} else if (endGame(columnB,rowB) == 4) {
			drawMap(96,0,(columnB-1)*8,rowB*8,8,8);
			BlackClear(16,0,columnB*8,rowB*8,8,8);
			printMargines();
			printEndGameBlueWins();
			break;
		} else if (endGame(columnB,rowB) == 5) {
			drawMap(96,0,columnB*8,(rowB-1)*8,8,8);
			printEndGameBlueWins();
			break;
		}

		BlankMap[columnB][rowB] = BOT;

		p = movePlayer(&proslo_stanje,&trenutno_stanje);
		switch(p){
			case DOLE:
				rowP++;
				drawMap(16,0,columnP*8,rowP*8,8,8);
				drawMap(88,0,columnP*8,(rowP-1)*8,8,8);
				break;
			case GORE:
				rowP--;
				drawMap(0,0,columnP*8,rowP*8,8,8);
				drawMap(88,0,columnP*8,(rowP+1)*8,8,8);
				break;
			case LEVO:
				columnP--;
				drawMap(24,0,columnP*8,rowP*8,8,8);
				drawMap(80,0,(columnP+1)*8,rowP*8,8,8);
				break;
			case DESNO:
				columnP++;
				drawMap(8,0,columnP*8,rowP*8,8,8);
				drawMap(80,0,(columnP-1)*8,rowP*8,8,8);
				break;
		}


		if(endGame(columnP,rowP) == 1)
			{
				drawMap(96,0,columnP*8,rowP*8,8,8);
				printEndGameYellowWins();
				break;

			} else if (endGame(columnP,rowP) == 2) {
				drawMap(96,0,(columnP+1)*8,rowP*8,8,8);
				//BlackClear(16,0,columnP*8,rowP*8,8,8);
				printMargines();
				printEndGameYellowWins();
				break;
			} else if (endGame(columnP,rowP) == 3) {
				drawMap(96,0,columnP*8,(rowP+1)*8,8,8);
				printEndGameYellowWins();
				break;
			} else if (endGame(columnP,rowP) == 4) {
				drawMap(96,0,(columnP-1)*8,rowP*8,8,8);
				//BlackClear(16,0,columnP*8,rowP*8,8,8);
				printMargines();
				printEndGameYellowWins();
				break;
			} else if (endGame(columnP,rowP) == 5) {
				drawMap(96,0,columnP*8,(rowP-1)*8,8,8);
				printEndGameYellowWins();
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

PRAVAC moveBot(int* proslo_stanje,int *trenutno_stanje)
{

	int *p = proslo_stanje;
	int random;
	srand(time(0));

	if(*p == 1){
		random = rand()%3;
		if(random == 0)
		{
			*p = 1;
			return DESNO;
		}
		else if(random == 1)
		{
			*p = 3;
			return DOLE;
		}
		else if(random == 2)
		{
			*p = 4;
			return GORE;
		}
	}
	else if(*p == 2){
		random = rand()%3;
		if(random == 0)
		{
			*p = 2;
			return LEVO;
		}
		else if(random == 1)
		{
			*p = 3;
			return DOLE;
		}
		else if(random == 2)
		{
			*p = 4;
			return GORE;
		}
	}
	else if(*p == 3){
		random = rand()%3;
		if(random == 0)
		{
			*p = 3;
			return DOLE;
		}
		else if(random == 1)
		{
			*p = 2;
			return LEVO;
		}
		else if(random == 2)
		{
			*p = 1;
			return DESNO;
		}
	}
	else if(*p == 4){
			random = rand()%3;
			if(random == 0)
			{
				*p = 4;
				return GORE;
			}
			else if(random == 1)
			{
				*p = 2;
				return LEVO;
			}
			else if(random == 2)
			{
				*p = 1;
				return DESNO;
			}
		}
}

PRAVAC movePlayer(int* proslo_stanje,int *trenutno_stanje)
{
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
			if(*p == 2 && *t == 1)
			{
				pravac = LEVO;
				*p = 2;
			}else if(*p == 1 && *t == 2)
			{
				pravac = DESNO;
				*p = 1;
			}else if(*p == 4 && *t == 3)
			{
				pravac = GORE;
				*p = 4;
			}else if(*p == 3 && *t == 4)
			{
				pravac = DOLE;
				*p = 3;
			}


		}

		return pravac;
}

int endGame(int column, int row)
{
	int ret = 0;

	if(BlankMap[column][row] == PLAYER_TRACE || BlankMap[column][row] == BOTS_TRACE || BlankMap[column][row] == PLAYER ){
		ret = 1;
	} else if(column <0) {
		ret = 2;
	} else if(row <0) {
		ret = 3;
	} else if(column>39) {
		ret = 4;
	} else if(row >29) {
		ret = 5;
	}

	return ret;


}

PRAVAC botSurvivalMode(int* proslo_stanje)
{
	int random;
	srand(time(0));
	random = rand() % 2;
	if(*proslo_stanje == 1 || *proslo_stanje == 2)
	{
		if(random == 0)
		{
			return GORE;
		}else{
			return DOLE;
		}
	}
	if(*proslo_stanje == 3 || *proslo_stanje == 2)
	{
		if(random == 0)
		{
			return LEVO;
		}else{
			return DESNO;
		}
	}
}

void printEndGameBlueWins()
{
	int brojac=0;
	while(brojac < 5000000){brojac++;}
	while(1) {
		int tc = rand()%40;
		int tr = rand()%30;
		drawMap(0,0,tc*8,tr*8,8,8);
	}
}

void printEndGameYellowWins()
{
	int brojac=0;
	while(brojac < 5000000){brojac++;}
	while(1) {
		int tc = rand()%40;
		int tr = rand()%30;
		drawMap(32,0,tc*8,tr*8,8,8);
	}
}






