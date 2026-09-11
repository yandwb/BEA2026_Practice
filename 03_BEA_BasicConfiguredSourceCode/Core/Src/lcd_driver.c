/* lcd_driver.c - ILI9341 TFT LCD Driver (Software SPI) for OpenX05R-C */
#include "lcd_driver.h"

/* ---- Bit-bang SPI macros ---- */
#define CS_LOW()    HAL_GPIO_WritePin(LCD_CS_GPIO,   LCD_CS_PIN,   GPIO_PIN_RESET)
#define CS_HIGH()   HAL_GPIO_WritePin(LCD_CS_GPIO,   LCD_CS_PIN,   GPIO_PIN_SET)
#define DC_CMD()    HAL_GPIO_WritePin(LCD_DC_GPIO,   LCD_DC_PIN,   GPIO_PIN_RESET)
#define DC_DATA()   HAL_GPIO_WritePin(LCD_DC_GPIO,   LCD_DC_PIN,   GPIO_PIN_SET)
#define SCK_LOW()   HAL_GPIO_WritePin(LCD_SCK_GPIO,  LCD_SCK_PIN,  GPIO_PIN_RESET)
#define SCK_HIGH()  HAL_GPIO_WritePin(LCD_SCK_GPIO,  LCD_SCK_PIN,  GPIO_PIN_SET)
#define MOSI_LOW()  HAL_GPIO_WritePin(LCD_MOSI_GPIO, LCD_MOSI_PIN, GPIO_PIN_RESET)
#define MOSI_HIGH() HAL_GPIO_WritePin(LCD_MOSI_GPIO, LCD_MOSI_PIN, GPIO_PIN_SET)

/* ---- Simple 5x7 ASCII font (chars 0x20 to 0x7E) ---- */
static const uint8_t font5x7[][5] = {
    {0x00,0x00,0x00,0x00,0x00},/*  */
    {0x00,0x00,0x5F,0x00,0x00},/*!*/
    {0x00,0x07,0x00,0x07,0x00},/*"*/
    {0x14,0x7F,0x14,0x7F,0x14},/*#*/
    {0x24,0x2A,0x7F,0x2A,0x12},/*$*/
    {0x23,0x13,0x08,0x64,0x62},/*%*/
    {0x36,0x49,0x55,0x22,0x50},/*&*/
    {0x00,0x05,0x03,0x00,0x00},/*'*/
    {0x00,0x1C,0x22,0x41,0x00},/*(*/
    {0x00,0x41,0x22,0x1C,0x00},/*)*/
    {0x14,0x08,0x3E,0x08,0x14},/***/
    {0x08,0x08,0x3E,0x08,0x08},/*+*/
    {0x00,0x50,0x30,0x00,0x00},/*,*/
    {0x08,0x08,0x08,0x08,0x08},/*-*/
    {0x00,0x60,0x60,0x00,0x00},/*.*/
    {0x20,0x10,0x08,0x04,0x02},/*/*/
    {0x3E,0x51,0x49,0x45,0x3E},/*0*/
    {0x00,0x42,0x7F,0x40,0x00},/*1*/
    {0x42,0x61,0x51,0x49,0x46},/*2*/
    {0x21,0x41,0x45,0x4B,0x31},/*3*/
    {0x18,0x14,0x12,0x7F,0x10},/*4*/
    {0x27,0x45,0x45,0x45,0x39},/*5*/
    {0x3C,0x4A,0x49,0x49,0x30},/*6*/
    {0x01,0x71,0x09,0x05,0x03},/*7*/
    {0x36,0x49,0x49,0x49,0x36},/*8*/
    {0x06,0x49,0x49,0x29,0x1E},/*9*/
    {0x00,0x36,0x36,0x00,0x00},/*:*/
    {0x00,0x56,0x36,0x00,0x00},/*;*/
    {0x08,0x14,0x22,0x41,0x00},/*<*/
    {0x14,0x14,0x14,0x14,0x14},/*=*/
    {0x00,0x41,0x22,0x14,0x08},/*>*/
    {0x02,0x01,0x51,0x09,0x06},/*?*/
    {0x32,0x49,0x79,0x41,0x3E},/*@*/
    {0x7E,0x11,0x11,0x11,0x7E},/*A*/
    {0x7F,0x49,0x49,0x49,0x36},/*B*/
    {0x3E,0x41,0x41,0x41,0x22},/*C*/
    {0x7F,0x41,0x41,0x22,0x1C},/*D*/
    {0x7F,0x49,0x49,0x49,0x41},/*E*/
    {0x7F,0x09,0x09,0x09,0x01},/*F*/
    {0x3E,0x41,0x49,0x49,0x7A},/*G*/
    {0x7F,0x08,0x08,0x08,0x7F},/*H*/
    {0x00,0x41,0x7F,0x41,0x00},/*I*/
    {0x20,0x40,0x41,0x3F,0x01},/*J*/
    {0x7F,0x08,0x14,0x22,0x41},/*K*/
    {0x7F,0x40,0x40,0x40,0x40},/*L*/
    {0x7F,0x02,0x0C,0x02,0x7F},/*M*/
    {0x7F,0x04,0x08,0x10,0x7F},/*N*/
    {0x3E,0x41,0x41,0x41,0x3E},/*O*/
    {0x7F,0x09,0x09,0x09,0x06},/*P*/
    {0x3E,0x41,0x51,0x21,0x5E},/*Q*/
    {0x7F,0x09,0x19,0x29,0x46},/*R*/
    {0x46,0x49,0x49,0x49,0x31},/*S*/
    {0x01,0x01,0x7F,0x01,0x01},/*T*/
    {0x3F,0x40,0x40,0x40,0x3F},/*U*/
    {0x1F,0x20,0x40,0x20,0x1F},/*V*/
    {0x3F,0x40,0x38,0x40,0x3F},/*W*/
    {0x63,0x14,0x08,0x14,0x63},/*X*/
    {0x07,0x08,0x70,0x08,0x07},/*Y*/
    {0x61,0x51,0x49,0x45,0x43},/*Z*/
    {0x00,0x7F,0x41,0x41,0x00},/*[*/
    {0x02,0x04,0x08,0x10,0x20},/*\*/
    {0x00,0x41,0x41,0x7F,0x00},/*]*/
    {0x04,0x02,0x01,0x02,0x04},/*^*/
    {0x40,0x40,0x40,0x40,0x40},/*_*/
    {0x00,0x01,0x02,0x04,0x00},/*`*/
    {0x20,0x54,0x54,0x54,0x78},/*a*/
    {0x7F,0x48,0x44,0x44,0x38},/*b*/
    {0x38,0x44,0x44,0x44,0x20},/*c*/
    {0x38,0x44,0x44,0x48,0x7F},/*d*/
    {0x38,0x54,0x54,0x54,0x18},/*e*/
    {0x08,0x7E,0x09,0x01,0x02},/*f*/
    {0x0C,0x52,0x52,0x52,0x3E},/*g*/
    {0x7F,0x08,0x04,0x04,0x78},/*h*/
    {0x00,0x44,0x7D,0x40,0x00},/*i*/
    {0x20,0x40,0x44,0x3D,0x00},/*j*/
    {0x7F,0x10,0x28,0x44,0x00},/*k*/
    {0x00,0x41,0x7F,0x40,0x00},/*l*/
    {0x7C,0x04,0x18,0x04,0x78},/*m*/
    {0x7C,0x08,0x04,0x04,0x78},/*n*/
    {0x38,0x44,0x44,0x44,0x38},/*o*/
    {0x7C,0x14,0x14,0x14,0x08},/*p*/
    {0x08,0x14,0x14,0x18,0x7C},/*q*/
    {0x7C,0x08,0x04,0x04,0x08},/*r*/
    {0x48,0x54,0x54,0x54,0x20},/*s*/
    {0x04,0x3F,0x44,0x40,0x20},/*t*/
    {0x3C,0x40,0x40,0x20,0x7C},/*u*/
    {0x1C,0x20,0x40,0x20,0x1C},/*v*/
    {0x3C,0x40,0x30,0x40,0x3C},/*w*/
    {0x44,0x28,0x10,0x28,0x44},/*x*/
    {0x0C,0x50,0x50,0x50,0x3C},/*y*/
    {0x44,0x64,0x54,0x4C,0x44},/*z*/
};

/* ---- Internal SPI write ---- */
static void spi_write_byte(uint8_t data)
{
    for (int i = 7; i >= 0; i--)
    {
        SCK_LOW();
        if (data & (1 << i)) MOSI_HIGH(); else MOSI_LOW();
        SCK_HIGH();
    }
}

static void lcd_write_cmd(uint8_t cmd)
{
    CS_LOW(); DC_CMD();
    spi_write_byte(cmd);
    CS_HIGH();
}

static void lcd_write_data8(uint8_t data)
{
    CS_LOW(); DC_DATA();
    spi_write_byte(data);
    CS_HIGH();
}

static void lcd_write_data16(uint16_t data)
{
    CS_LOW(); DC_DATA();
    spi_write_byte(data >> 8);
    spi_write_byte(data & 0xFF);
    CS_HIGH();
}

/* ---- Set drawing window ---- */
static void lcd_set_window(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
    lcd_write_cmd(0x2A);
    lcd_write_data8(x0 >> 8); lcd_write_data8(x0 & 0xFF);
    lcd_write_data8(x1 >> 8); lcd_write_data8(x1 & 0xFF);

    lcd_write_cmd(0x2B);
    lcd_write_data8(y0 >> 8); lcd_write_data8(y0 & 0xFF);
    lcd_write_data8(y1 >> 8); lcd_write_data8(y1 & 0xFF);

    lcd_write_cmd(0x2C);
}

/* ---- GPIO Init for LCD pins ---- */
static void lcd_gpio_init(void)
{
    GPIO_InitTypeDef g = {0};
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    /* PA7 = MOSI */
    g.Pin   = LCD_MOSI_PIN;
    g.Mode  = GPIO_MODE_OUTPUT_PP;
    g.Speed = GPIO_SPEED_FREQ_HIGH;
    g.Pull  = GPIO_NOPULL;
    HAL_GPIO_Init(LCD_MOSI_GPIO, &g);

    /* PB3 = SCK, PB7 = CS, PB8 = DC (SPI pins - HIGH speed) */
    g.Pin = LCD_SCK_PIN | LCD_CS_PIN | LCD_DC_PIN;
    g.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(LCD_SCK_GPIO, &g);

    /* PB6 = Backlight (LOW speed, PULLUP to stay ON) */
    g.Pin   = LCD_BL_PIN;
    g.Speed = GPIO_SPEED_FREQ_LOW;
    g.Pull  = GPIO_PULLUP;
    HAL_GPIO_Init(LCD_BL_GPIO, &g);

    /* Default states */
    CS_HIGH();
    SCK_LOW();
    MOSI_LOW();
    DC_DATA();
    /* Backlight ON */
    HAL_GPIO_WritePin(LCD_BL_GPIO, LCD_BL_PIN, GPIO_PIN_SET);
}


/* ---- ILI9341 Init Sequence ---- */
void LCD_Init(void)
{
    lcd_gpio_init(); /* gpio_init se test ca LOW/HIGH cho backlight */

    lcd_write_cmd(0x01); HAL_Delay(150); /* Software Reset */
    lcd_write_cmd(0x11); HAL_Delay(150); /* Sleep Out - Wake up */

    /* Power Control */
    lcd_write_cmd(0xCF);
    lcd_write_data8(0x00); lcd_write_data8(0xC1); lcd_write_data8(0x30);

    lcd_write_cmd(0xED);
    lcd_write_data8(0x64); lcd_write_data8(0x03); lcd_write_data8(0x12); lcd_write_data8(0x81);

    lcd_write_cmd(0xE8);
    lcd_write_data8(0x85); lcd_write_data8(0x00); lcd_write_data8(0x78);

    lcd_write_cmd(0xCB);
    lcd_write_data8(0x39); lcd_write_data8(0x2C); lcd_write_data8(0x00);
    lcd_write_data8(0x34); lcd_write_data8(0x02);

    lcd_write_cmd(0xF7); lcd_write_data8(0x20);

    lcd_write_cmd(0xEA); lcd_write_data8(0x00); lcd_write_data8(0x00);

    lcd_write_cmd(0xC0); lcd_write_data8(0x23); /* Power Control 1 */
    lcd_write_cmd(0xC1); lcd_write_data8(0x10); /* Power Control 2 */

    lcd_write_cmd(0xC5); lcd_write_data8(0x3E); lcd_write_data8(0x28); /* VCOM Control 1 */
    lcd_write_cmd(0xC7); lcd_write_data8(0x86); /* VCOM Control 2 */

    /* Memory Access Control: portrait mode (MV=1), RGB=0 -> 0x20 */
    lcd_write_cmd(0x36); lcd_write_data8(0x20);

    /* Turn ON Invert (IPS panel: correct colors) */
    lcd_write_cmd(0x21); 

    /* Pixel Format: 16-bit RGB565 */
    lcd_write_cmd(0x3A); lcd_write_data8(0x55);

    lcd_write_cmd(0xB1); lcd_write_data8(0x00); lcd_write_data8(0x18); /* Frame Rate */
    lcd_write_cmd(0xB6); lcd_write_data8(0x08); lcd_write_data8(0x82); lcd_write_data8(0x27);

    lcd_write_cmd(0xF2); lcd_write_data8(0x00);
    lcd_write_cmd(0x26); lcd_write_data8(0x01);

    /* Gamma curves */
    lcd_write_cmd(0xE0);
    lcd_write_data8(0x0F); lcd_write_data8(0x31); lcd_write_data8(0x2B);
    lcd_write_data8(0x0C); lcd_write_data8(0x0E); lcd_write_data8(0x08);
    lcd_write_data8(0x4E); lcd_write_data8(0xF1); lcd_write_data8(0x37);
    lcd_write_data8(0x07); lcd_write_data8(0x10); lcd_write_data8(0x03);
    lcd_write_data8(0x0E); lcd_write_data8(0x09); lcd_write_data8(0x00);

    lcd_write_cmd(0xE1);
    lcd_write_data8(0x00); lcd_write_data8(0x0E); lcd_write_data8(0x14);
    lcd_write_data8(0x03); lcd_write_data8(0x11); lcd_write_data8(0x07);
    lcd_write_data8(0x31); lcd_write_data8(0xC1); lcd_write_data8(0x48);
    lcd_write_data8(0x08); lcd_write_data8(0x0F); lcd_write_data8(0x0C);
    lcd_write_data8(0x31); lcd_write_data8(0x36); lcd_write_data8(0x0F);

    lcd_write_cmd(0x11); HAL_Delay(120); /* Sleep Out */
    lcd_write_cmd(0x29);                 /* Display ON */
    HAL_Delay(20);

    LCD_Clear(LCD_BLACK);
}

/* ---- Fill entire screen ---- */
void LCD_Clear(uint16_t color)
{
    lcd_set_window(0, 0, LCD_W - 1, LCD_H - 1);
    CS_LOW(); DC_DATA();
    for (uint32_t i = 0; i < (uint32_t)LCD_W * LCD_H; i++)
    {
        spi_write_byte(color >> 8);
        spi_write_byte(color & 0xFF);
    }
    CS_HIGH();
}

/* ---- Fill rectangle ---- */
void LCD_FillRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color)
{
    lcd_set_window(x, y, x + w - 1, y + h - 1);
    CS_LOW(); DC_DATA();
    for (uint32_t i = 0; i < (uint32_t)w * h; i++)
    {
        spi_write_byte(color >> 8);
        spi_write_byte(color & 0xFF);
    }
    CS_HIGH();
}

/* ---- Draw one character (Fast Streaming - Anti Tearing) ---- */
void LCD_DrawChar(uint16_t x, uint16_t y, char c, uint16_t fg, uint16_t bg, uint8_t size)
{
    if (c < 0x20 || c > 0x7A) c = ' ';
    const uint8_t *chr = font5x7[c - 0x20];

    /* Set 1 single window for the entire character */
    lcd_set_window(x, y, x + 6 * size - 1, y + 7 * size - 1);
    
    CS_LOW(); DC_DATA();
    /* Restore standard row-by-row streaming */
    for (uint8_t row = 0; row < 7 * size; row++)
    {
        for (uint8_t col = 0; col < 6 * size; col++)
        {
            uint8_t pixel_col = col / size;
            uint8_t pixel_row = row / size;
            uint16_t color = bg;
            
            if (pixel_col < 5) {
                if (chr[pixel_col] & (1 << pixel_row)) color = fg;
            }
            
            /* Stream directly to GRAM */
            spi_write_byte(color >> 8);
            spi_write_byte(color & 0xFF);
        }
    }
    CS_HIGH();
}

/* ---- Draw string ---- */
void LCD_DrawString(uint16_t x, uint16_t y, const char *str, uint16_t fg, uint16_t bg, uint8_t size)
{
    while (*str)
    {
        LCD_DrawChar(x, y, *str, fg, bg, size);
        x += 6 * size;
        str++;
    }
}

/* ---- 3-Tier Dashboard and Terminal ---- */
static uint16_t current_log_y = 136; /* Starts below Tier 2 */

void LCD_InitDashboard(void)
{
    LCD_FillRect(0, 0, 240, 20, LCD_NAVY); /* Chiều cao 20px, để hở 4px màu đen làm khoảng cách tới Node 1 */
    LCD_DrawString(4, 4, "BOSCH - Nhom 11", LCD_YELLOW, LCD_NAVY, 2);
    
    /* Tier 1: Node 1 (Receiver) */
    LCD_FillRect(0, 24, 240, 16, LCD_NAVY);
    LCD_DrawString(0, 24, "NODE1 - Receiver", LCD_CYAN, LCD_NAVY, 2);
    LCD_FillRect(0, 40, 240, 16, LCD_BLACK);
    LCD_DrawString(0, 40, "RX 0A2: -- -- -- -- -- -- -- --", LCD_WHITE, LCD_BLACK, 1);
    LCD_DrawString(0, 48, "TX 012: -- -- -- -- -- -- -- --", LCD_WHITE, LCD_BLACK, 1);
    
    /* Tier 2: Node 2 (Transmitter) */
    LCD_FillRect(0, 64, 240, 16, LCD_NAVY);
    LCD_DrawString(0, 64, "NODE2 - Transmitter", LCD_GREEN, LCD_NAVY, 2);
    LCD_FillRect(0, 80, 240, 32, LCD_BLACK);
    LCD_DrawString(0, 80, "TX 0A2: -- -- -- -- -- -- -- --", LCD_WHITE, LCD_BLACK, 1);
    LCD_DrawString(0, 88, "RX 012: -- -- -- -- -- -- -- --", LCD_WHITE, LCD_BLACK, 1);
    LCD_DrawString(0, 96, "CRC: -- (WAIT)", LCD_WHITE, LCD_BLACK, 2);
    
    /* Tier 3: Log */
    LCD_FillRect(0, 120, 240, 200, LCD_BLACK);
    LCD_DrawString(0, 120, "[ CANlog Recording ]", LCD_CYAN, LCD_BLACK, 2);
}

void LCD_UpdateNode1(uint8_t is_tx, uint8_t *data)
{
    char buf[44];
    if (!is_tx) {
        sprintf(buf, "RX 0A2: %02X %02X %02X %02X %02X %02X %02X %02X", 
                data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7]);
        LCD_FillRect(0, 40, 240, 8, LCD_BLACK);
        LCD_DrawString(0, 40, buf, LCD_WHITE, LCD_BLACK, 1);
    } else {
        sprintf(buf, "TX 012: %02X %02X %02X %02X %02X %02X %02X %02X", 
                data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7]);
        LCD_FillRect(0, 48, 240, 8, LCD_BLACK);
        LCD_DrawString(0, 48, buf, LCD_WHITE, LCD_BLACK, 1);
    }
}

void LCD_UpdateNode2(uint8_t is_tx, uint8_t *data, uint8_t crc, uint8_t crc_ok)
{
    char buf[44];
    if (is_tx) {
        sprintf(buf, "TX 0A2: %02X %02X %02X %02X %02X %02X %02X %02X", 
                data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7]);
        LCD_FillRect(0, 80, 240, 8, LCD_BLACK);
        LCD_DrawString(0, 80, buf, LCD_WHITE, LCD_BLACK, 1);
    } else {
        sprintf(buf, "RX 012: %02X %02X %02X %02X %02X %02X %02X %02X", 
                data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7]);
        LCD_FillRect(0, 88, 240, 8, LCD_BLACK);
        LCD_DrawString(0, 88, buf, LCD_WHITE, LCD_BLACK, 1);
        
        sprintf(buf, "CRC: %02X (%s)", crc, crc_ok ? "OK" : "FL");
        LCD_FillRect(0, 96, 240, 16, LCD_BLACK);
        LCD_DrawString(0, 96, buf, crc_ok ? LCD_GREEN : LCD_RED, LCD_BLACK, 2);
    }
}

#include <string.h>
#define LOG_MAX_LINES 23
static char log_lines[LOG_MAX_LINES][44];
static uint8_t log_count = 0;

void LCD_AddLog(uint8_t *data)
{
    char buf[44];
    sprintf(buf, " RX 0A2: %02X %02X %02X %02X %02X %02X %02X %02X", 
            data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7]);
    
    if (log_count < LOG_MAX_LINES) {
        strcpy(log_lines[log_count], buf);
        LCD_DrawString(0, 136 + log_count * 8, log_lines[log_count], LCD_WHITE, LCD_BLACK, 1);
        log_count++;
    } else {
        /* Scroll up all lines */
        for(int i = 0; i < LOG_MAX_LINES - 1; i++) {
            strcpy(log_lines[i], log_lines[i+1]);
            LCD_DrawString(0, 136 + i * 8, log_lines[i], LCD_WHITE, LCD_BLACK, 1);
        }
        /* Insert new line at bottom */
        strcpy(log_lines[LOG_MAX_LINES - 1], buf);
        LCD_DrawString(0, 136 + (LOG_MAX_LINES - 1) * 8, log_lines[LOG_MAX_LINES - 1], LCD_WHITE, LCD_BLACK, 1);
    }
}
