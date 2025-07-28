#include "lcd.h"
#include "eeprom.h"
#include "font.h"
#include "fw_hal.h"

#define CMD_SET_X 0x2A  // 设置X坐标
#define CMD_SET_Y 0x2B  // 设置Y坐标
#define CMD_WR_RAM 0x2C // 写GRAM

#define RGB565(r, g, b) ((uint16_t)((((uint8_t)(r) >> 3) << 11) | (((uint8_t)(g) >> 2) << 5) | ((uint8_t)(b) >> 3)))

#define COLOR_WHITE RGB565(255, 255, 255)
#define COLOR_BLACK RGB565(0, 0, 0)
#define COLOR_BLUE RGB565(0x34, 0x6d, 0xf1)
#define COLOR_GREEN RGB565(0, 255, 0)
#define COLOR_RED RGB565(255, 60, 60)
#define COLOR_GRAY RGB565(0x00, 0xff, 0x00)

#define FONT_HEIGHT 26

static uint16_t COLOR_MAP[] = {COLOR_BLACK, COLOR_WHITE, COLOR_GREEN, COLOR_RED, COLOR_BLUE};

static uint8_t font_bmp[52];

#define CHINESE_WIDTH 26
#define CHINESE_BYTES 85

#define DIGIT_WIDTH 16
#define DIGIT_BYTES 52

static void writeBus(uint8_t data) {
    for (uint8_t i = 0; i < 8; ++i) {
        SPI_SCK = 0;
        if (data & 0x80) {
            SPI_SDI = 1;
        } else {
            SPI_SDI = 0;
        }
        SPI_SCK = 1;
        data <<= 1;
    }
}

static void LCD_WR_REG(uint8_t reg) {
    SPI_DC = 0;
    SPI_CS = 0;
    writeBus(reg);
    SPI_CS = 1;
}

static void LCD_WR_DATA(uint8_t data) {
    SPI_DC = 1;
    SPI_CS = 0;
    writeBus(data);
    SPI_CS = 1;
}

// 开始写GRAM
static void writeRAMPrepare(void) { LCD_WR_REG(CMD_WR_RAM); }

void setWindow(uint8_t sx, uint8_t sy, uint8_t width, uint8_t height) {
    uint16_t ex, ey;
    ex = sx + width - 1;
    ey = sy + height - 1;

#if LCD_DIR == LCD_DIR_0
    sx = sx + 26;
    ex = ex + 26;
    sy = sy + 1;
    ey = ey + 1;
#elif LCD_DIR == LCD_DIR_180
    sx = sx + 26;
    ex = ex + 26;
    sy = sy + 1;
    ey = ey + 1;
#elif LCD_DIR == LCD_DIR_270
    sx = sx + 1;
    ex = ex + 1;
    sy = sy + 26;
    ey = ey + 26;
#elif LCD_DIR == LCD_DIR_90
    sx = sx + 1;
    ex = ex + 1;
    sy = sy + 26;
    ey = ey + 26;
#endif

    LCD_WR_REG(CMD_SET_X);
    LCD_WR_DATA(0);
    LCD_WR_DATA(sx);
    LCD_WR_DATA(0);
    LCD_WR_DATA(ex);
    LCD_WR_REG(CMD_SET_Y);
    LCD_WR_DATA(0);
    LCD_WR_DATA(sy);
    LCD_WR_DATA(0);
    LCD_WR_DATA(ey);
}

static void drawPoint(uint8_t x, uint8_t y, uint16_t color) {
    setWindow(x, y, 1, 1);
    writeRAMPrepare();
    SPI_DC = 1;
    SPI_CS = 0;
    writeBus(color >> 8);
    writeBus(color & 0xFF);
    SPI_CS = 1;
}

// 初始化lcd
void LCD_init(void) {
    // SPI_SetClockPrescaler(SPI_ClockPreScaler_4);
    // SPI_SetClockPolarity(HAL_State_ON);
    // SPI_SetClockPhase(SPI_ClockPhase_LeadingEdge);
    // SPI_SetDataOrder(SPI_DataOrder_MSB);
    // SPI_IgnoreSlaveSelect(HAL_State_OFF);
    // SPI_SetMasterMode(HAL_State_ON);
    // SPI_SetEnabled(HAL_State_ON);
    SPCTL = 0x58;

    // SPI_SetPort(SPI_AlterPort_P12P54_P13_P14_P15);
    P_SW1 = 0x00;

    SPI_RST = 1;
    SYS_Delay(300);

    LCD_WR_REG(0x11); // Sleep out

    SYS_Delay(120); // delay_ms 120ms

    LCD_WR_REG(0xB1); // Normal mode
    LCD_WR_DATA(0x05);
    LCD_WR_DATA(0x3C);
    LCD_WR_DATA(0x3C);

    LCD_WR_REG(0xC0); // AVDD GVDD
    LCD_WR_DATA(0x04);
    LCD_WR_DATA(0x04);
    LCD_WR_DATA(0x84);

    LCD_WR_REG(0xC1); // VGH VGL
    LCD_WR_DATA(0x06);

    LCD_WR_REG(0xC2); // Normal Mode
    LCD_WR_DATA(0x0D);
    LCD_WR_DATA(0x00);

    LCD_WR_REG(0xC5); // VCOM
    LCD_WR_DATA(0x3A);

    LCD_WR_REG(0xE0); // positive gamma
    LCD_WR_DATA(0x10);
    LCD_WR_DATA(0x0E);
    LCD_WR_DATA(0x05);
    LCD_WR_DATA(0x0D);
    LCD_WR_DATA(0x0E);
    LCD_WR_DATA(0x06);
    LCD_WR_DATA(0x03);
    LCD_WR_DATA(0x07);
    LCD_WR_DATA(0x0A);
    LCD_WR_DATA(0x0E);
    LCD_WR_DATA(0x1D);
    LCD_WR_DATA(0x32);
    LCD_WR_DATA(0x03);
    LCD_WR_DATA(0x0B);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x2F);

    LCD_WR_REG(0xE1); // negative gamma
    LCD_WR_DATA(0x0E);
    LCD_WR_DATA(0x12);
    LCD_WR_DATA(0x05);
    LCD_WR_DATA(0x07);
    LCD_WR_DATA(0x0C);
    LCD_WR_DATA(0x05);
    LCD_WR_DATA(0x02);
    LCD_WR_DATA(0x06);
    LCD_WR_DATA(0x07);
    LCD_WR_DATA(0x0B);
    LCD_WR_DATA(0x13);
    LCD_WR_DATA(0x2C);
    LCD_WR_DATA(0x03);
    LCD_WR_DATA(0x0D);
    LCD_WR_DATA(0x02);
    LCD_WR_DATA(0x1F);

    LCD_WR_REG(0x3A);
    LCD_WR_DATA(0x05);

#if LCD_DIR == LCD_DIR_0
    // 0度旋转
    LCD_WR_REG(0x36);
    LCD_WR_DATA((uint8_t)((1 << 3) | (1 << 7) | (1 << 6) | (0 << 5)));
#elif LCD_DIR == LCD_DIR_180
    // 180度旋转
    LCD_WR_REG(0x36);
    LCD_WR_DATA((uint8_t)((1 << 3) | (0 << 7) | (0 << 6) | (0 << 5)));
#elif LCD_DIR == LCD_DIR_270
    // 270度旋转
    LCD_WR_REG(0x36);
    LCD_WR_DATA((uint8_t)((1 << 3) | (0 << 7) | (1 << 6) | (1 << 5)));
#else
    // 90度旋转
    LCD_WR_REG(0x36);
    LCD_WR_DATA((uint8_t)((1 << 3) | (1 << 7) | (0 << 6) | (1 << 5)));
#endif

    LCD_WR_REG(0x21); // Display inversion
    LCD_WR_REG(0x29); // Display on
}

void LCD_clear(uint8_t color) { LCD_fill(0, 0, LCD_WIDTH, LCD_HEIGHT, color); }

void LCD_fill(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t color) {
    setWindow(x, y, width, height);
    writeRAMPrepare();
    SPI_DC = 1;
    SPI_CS = 0;
    for (uint8_t x = 0; x < LCD_WIDTH; ++x) {
        for (uint8_t y = 0; y < LCD_HEIGHT; ++y) {
            writeBus(COLOR_MAP[color] >> 8);
            writeBus(COLOR_MAP[color] & 0xFF);
        }
    }
    SPI_CS = 1;
}

void LCD_show_font_char(uint8_t x, uint8_t y, const uint8_t *font, uint8_t bytes, uint8_t color) {
    uint8_t y0 = y;
    for (uint8_t b = 0; b < bytes; b++) {
        uint8_t c = font[b];
        for (uint8_t i = 0; i < 8; i++) {
            if ((y - y0) >= FONT_HEIGHT) {
                x++;
                if (x >= LCD_WIDTH) {
                    return;
                }
                y = y0;
            }

            if (c & 0x80) {
                drawPoint(x, y, COLOR_MAP[color > 10 ? color - 10 : color]);
            } else {
                drawPoint(x, y, color >= 10 ? COLOR_GRAY : COLOR_BLACK);
            }
            c <<= 1;
            y++;
        }
    }
}

void LCD_show_number(uint8_t x, uint8_t y, uint16_t number, uint8_t width, uint8_t color) {
    x = x + DIGIT_WIDTH * (width - 1);
    if (number == 0) {
        EEPROM_read(FONT_DIGIT_ADDR, font_bmp, DIGIT_BYTES);
        LCD_show_font_char(x, y, font_bmp, DIGIT_BYTES, color);
        return;
    }
    while (width > 0) {
        uint8_t digit = number % 10;
        number /= 10;

        if (number == 0 && digit == 0) {
            LCD_fill(x, y, DIGIT_WIDTH, FONT_HEIGHT, BLACK);
        } else {
            EEPROM_read(FONT_DIGIT_ADDR + digit * DIGIT_BYTES, font_bmp, DIGIT_BYTES);
            LCD_show_font_char(x, y, font_bmp, DIGIT_BYTES, color);
        }
        width--;
        x -= DIGIT_WIDTH;
    }
}

void LCD_show_chinese(uint8_t x, uint8_t y, const uint8_t text_idx, uint8_t count, uint8_t color) {
    for (uint8_t i = 0; i < count; i++) {
        uint8_t idx = TEXT_IDX_ADDR[text_idx + i];

        if (idx == 0xff) {
            LCD_fill(x, y, CHINESE_WIDTH, FONT_HEIGHT, BLACK);
        } else {
            EEPROM_read(FONT_CHINESE_ADDR + idx * CHINESE_BYTES, font_bmp, 52);
            LCD_show_font_char(x, y, font_bmp, 52, color);

            EEPROM_read(FONT_CHINESE_ADDR + idx * CHINESE_BYTES + 52, font_bmp, 33);
            LCD_show_font_char(x + 16, y, font_bmp, 33, color);
        }

        x += CHINESE_WIDTH;
    }
}
