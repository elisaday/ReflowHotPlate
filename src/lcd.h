#ifndef __H_LCD__
#define __H_LCD__

#include "fw_hal.h"

#define SPI_SDI P13
#define SPI_SCK P15
#define SPI_DC P17
#define SPI_RST P16
#define SPI_CS P12

// 定义液晶屏顺时针旋转方向
#define LCD_DIR_0 0   // 0-0度旋转
#define LCD_DIR_180 1 // 180度旋转
#define LCD_DIR_270 2 // 270度旋转
#define LCD_DIR_90 3  // 90度旋转

#if LCD_DIR == LCD_DIR_0
#define LCD_WIDTH 80
#define LCD_HEIGHT 160
#elif LCD_DIR == LCD_DIR_180
#define LCD_WIDTH 80
#define LCD_HEIGHT 160
#elif LCD_DIR == LCD_DIR_270
#define LCD_WIDTH 160
#define LCD_HEIGHT 80
#else
#define LCD_WIDTH 160
#define LCD_HEIGHT 80
#endif

#define BLACK 0
#define WHITE 1
#define GREEN 2
#define RED 3
#define BLUE 4

#define GRAY_BG 10
#define BLACK_GRAY 10
#define WHITE_GRAY 11
#define RED_GRAY 12
#define GREEN_GRAY 13
#define BLUE_GRAY 14

void LCD_init(void);
void LCD_clear(uint8_t color);
void LCD_fill(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t color);
void LCD_show_font_char(uint8_t x, uint8_t y, const uint8_t *font, uint8_t bytes, uint8_t color);
void LCD_show_number(uint8_t x, uint8_t y, uint16_t number, uint8_t width, uint8_t color);
void LCD_show_chinese(uint8_t x, uint8_t y, const uint8_t text_idx, uint8_t count, uint8_t color);

#endif
