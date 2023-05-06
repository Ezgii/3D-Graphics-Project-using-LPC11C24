/*
 * lcd__api.h
 *
 *  Created on: Sep 30, 2022
 *      Author: brucejiang
 */
#pragma once
#include <stdint.h>

// TFT LCD properties
#define ST7735_TFTWIDTH 127
#define ST7735_TFTHEIGHT 159

// TFT LCD opcode commend
#define ST7735_CASET 0x2A
#define ST7735_RASET 0x2B
#define ST7735_RAMWR 0x2C
#define ST7735_SLPOUT 0x11
#define ST7735_DISPON 0x29

// Defining color values
#define LIGHTBLUE 0x00FFE0
#define GREEN 0x00FF00
#define DARKBLUE 0x000033
#define BLACK 0x000000
#define BLUE 0x0007FF
#define RED 0xFF0000
#define MAGENTA 0x00F81F
#define WHITE 0xFFFFFF
#define PURPLE 0xCC33FF
#define DARKBROWN 0x783F04
#define BROWN 0x8A2A2A
#define LIGHTBROWN 0xa34120
#define DARKGREEN 0x006400
#define YELLOW 0xFED000


/* public functions */
void fillrect(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint32_t color);
void lcddelay(int ms);
void lcd_init();
void drawPixel(int16_t x, int16_t y, uint32_t color);
void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint32_t color);
void board_init(void);
void ssp0_init(void);

