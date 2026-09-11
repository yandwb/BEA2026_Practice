/* lcd_driver.h - ILI9341 TFT LCD Driver for OpenX05R-C board */
#ifndef LCD_DRIVER_H_
#define LCD_DRIVER_H_

#include "stm32f4xx_hal.h"
#include <stdint.h>
#include <stdio.h>

/* PIN DEFINITIONS (from OpenX05R-C schematic) */
#define LCD_SCK_GPIO    GPIOB
#define LCD_SCK_PIN     GPIO_PIN_3   /* PB3 - SPI1_SCK  */
#define LCD_MOSI_GPIO   GPIOA
#define LCD_MOSI_PIN    GPIO_PIN_7   /* PA7 - SPI1_MOSI */
#define LCD_CS_GPIO     GPIOB
#define LCD_CS_PIN      GPIO_PIN_7   /* PB7 - LCD_CS    */
#define LCD_DC_GPIO     GPIOB
#define LCD_DC_PIN      GPIO_PIN_8   /* PB8 - LCD_RS    */
#define LCD_BL_GPIO     GPIOB
#define LCD_BL_PIN      GPIO_PIN_6   /* PB6 - LCD_PWM (backlight) */

/* SCREEN DIMENSIONS */
#define LCD_W   240
#define LCD_H   320

/* RGB565 COLORS */
#define LCD_BLACK     0x0000
#define LCD_WHITE     0xFFFF
#define LCD_RED       0xF800
#define LCD_GREEN     0x07E0
#define LCD_BLUE      0x001F
#define LCD_YELLOW    0xFFE0
#define LCD_CYAN      0x07FF
#define LCD_ORANGE    0xFD20
#define LCD_DARKGRAY  0x39E7
#define LCD_LIGHTGRAY 0xC618
#define LCD_NAVY      0x000F

/* PUBLIC API */
void LCD_Init(void);
void LCD_Clear(uint16_t color);
void LCD_FillRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
void LCD_DrawChar(uint16_t x, uint16_t y, char c, uint16_t fg, uint16_t bg, uint8_t size);
void LCD_DrawString(uint16_t x, uint16_t y, const char *str, uint16_t fg, uint16_t bg, uint8_t size);
void LCD_InitDashboard(void);
void LCD_UpdateNode1(uint8_t is_tx, uint8_t *data);
void LCD_UpdateNode2(uint8_t is_tx, uint8_t *data, uint8_t crc, uint8_t crc_ok);
void LCD_AddLog(uint8_t *data);

#endif /* LCD_DRIVER_H_ */
