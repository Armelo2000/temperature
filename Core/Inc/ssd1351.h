/*
 * ssd1351.h
 *
 *  Created on: Jan 9, 2023
 *      Author: Usman
 *      https://github.com/afiskon/stm32-ssd1351/tree/master/Lib/ssd1351
 */

#ifndef INC_SSD1351_H_
#define INC_SSD1351_H_


#include "main.h"
#include "fonts.h"
#include <stdbool.h>

//Port/Pin definition
#define SSD1351_SPI_PORT hspi3
extern SPI_HandleTypeDef SSD1351_SPI_PORT;

#define SSD1351_RES_Pin       RST_Pin
#define SSD1351_RES_GPIO_Port RST_GPIO_Port
#define SSD1351_CS_Pin        CS_Pin
#define SSD1351_CS_GPIO_Port  CS_GPIO_Port
#define SSD1351_DC_Pin        D_C_Pin
#define SSD1351_DC_GPIO_Port  D_C_GPIO_Port

// SCREEN WIDTH/HEIGHT
#define SSD1351_WIDTH  128
#define SSD1351_HEIGHT 128


// Color definitions
#define	SSD1351_BLACK   0x0000
#define	SSD1351_RED     0x001F
#define	SSD1351_BLUE    0xF800
#define SSD1351_WHITE   0xFFFF




/*! \brief Function SSD1351_Unselect() will set the CS_Pin to
 *         high and make the Chip Select inactive
 */
void SSD1351_Unselect();


/*! \brief Function SSD1351_Init(void) will initializate the
 *         OLED click. Need done before using OLED
 */
void SSD1351_Init(void);


/*! \brief Function SSD1351_WriteString(uint16_t x, uint16_t y, const char* str, FontDef font, uint16_t color, uint16_t bgcolor) will
 *         write a Text/string on the OLED display with adjustable position and text size
 *  \param x is the x coordinate of the beginning of the text
 *  \param y is the y coordinate of the beginning f the text
 *  \param *str is the text that should be displayed
 *  \param font is the size of the text
 *  \param color is the color of the text
 *  \param bgcolor is the backroundcolor of the text
 */
void SSD1351_WriteString(uint16_t x, uint16_t y, const char* str, FontDef font, uint16_t color, uint16_t bgcolor);



/*! \brief Function SSD1351_FillRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color) will
 *         fill a rectangle  with a specific color
 *  \param x is the x coordinate of the beginning of the rectangle
 *  \param y is the y coordinate of the beginning of the rectangle
 *  \param w is the width
 *  \param h is the height
 *  \param color is the color of the rectangle that shoulde be filled
 */
void SSD1351_FillRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);



/*! \brief Function SSD1351_FillScreen(uint16_t color) will fill
 *         the hole screen in a specific color
 *  \param color is the color of the hole screen
 */
void SSD1351_FillScreen(uint16_t color);




/*! \brief Function display_output(float result, char buf[],char buf2[]) will display
 *         the temperature in celcius and in fahrenheit, if the temperature is under 15°C
 *         it should display blue, if it between 15^C - 28°C displayed white and if it is
 *         over 28°C it is red
 *  \param result is the temperature in °C
 *  \param buf[] is the temperature as a string in °C
 *  \param buf2[] is the temperature as a string in °F
 *  \returns 0 upon success, >0 on error
 */
void display_output(float result, char buf[],char buf2[]);

#endif /* INC_SSD1351_H_ */
