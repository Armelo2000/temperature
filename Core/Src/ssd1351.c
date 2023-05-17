/*
 * ssd1351.c
 *
 *  Created on: Jan 9, 2023
 *      Author: Usman
 *      https://github.com/afiskon/stm32-ssd1351/tree/master/Lib/ssd1351
 */


#include "stm32l4xx_hal.h"
#include "ssd1351.h"


// set CS_Pin low
static void SSD1351_Select() {
    HAL_GPIO_WritePin(SSD1351_CS_GPIO_Port, SSD1351_CS_Pin, GPIO_PIN_RESET);
}

//set CS_PIN high
void SSD1351_Unselect() {
    HAL_GPIO_WritePin(SSD1351_CS_GPIO_Port, SSD1351_CS_Pin, GPIO_PIN_SET);
}

//Reset Sequence
static void SSD1351_Reset() {
    HAL_GPIO_WritePin(SSD1351_RES_GPIO_Port, SSD1351_RES_Pin, GPIO_PIN_SET);
    HAL_Delay(500);
    HAL_GPIO_WritePin(SSD1351_RES_GPIO_Port, SSD1351_RES_Pin, GPIO_PIN_RESET);
    HAL_Delay(500);
    HAL_GPIO_WritePin(SSD1351_RES_GPIO_Port, SSD1351_RES_Pin, GPIO_PIN_SET);
    HAL_Delay(500);
}

//write a Command
static void SSD1351_WriteCommand(uint8_t cmd) {
    HAL_GPIO_WritePin(SSD1351_DC_GPIO_Port, SSD1351_DC_Pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(&SSD1351_SPI_PORT, &cmd, sizeof(cmd), HAL_MAX_DELAY);
}

//write Data
static void SSD1351_WriteData(uint8_t* buff, size_t buff_size) {
    HAL_GPIO_WritePin(SSD1351_DC_GPIO_Port, SSD1351_DC_Pin, GPIO_PIN_SET);

    // split data in small chunks because HAL can't send more then 64K at once
    while(buff_size > 0) {
        uint16_t chunk_size = buff_size > 32768 ? 32768 : buff_size;
        HAL_SPI_Transmit(&SSD1351_SPI_PORT, buff, chunk_size, HAL_MAX_DELAY);
        buff += chunk_size;
        buff_size -= chunk_size;
    }
}

//Set column and row
static void SSD1351_SetAddressWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
    // column address set
    SSD1351_WriteCommand(0x15); // SETCOLUMN
    {
        uint8_t data[] = { x0 & 0xFF, x1 & 0xFF };
        SSD1351_WriteData(data, sizeof(data));
    }

    // row address set
    SSD1351_WriteCommand(0x75); // SETROW
    {
        uint8_t data[] = { y0 & 0xFF, y1 & 0xFF };
        SSD1351_WriteData(data, sizeof(data));
    }

    // write to RAM
    SSD1351_WriteCommand(0x5C); // WRITERAM
}

//OLED initialization
void SSD1351_Init() {
    SSD1351_Select();
    SSD1351_Reset();

    SSD1351_WriteCommand(0xFD); // COMMANDLOCK
    {
        uint8_t data[] = { 0x12 };
        SSD1351_WriteData(data, sizeof(data));
    }
    SSD1351_WriteCommand(0xFD); // COMMANDLOCK
    {
        uint8_t data[] = { 0xB1 };
        SSD1351_WriteData(data, sizeof(data));
    }
    SSD1351_WriteCommand(0xAE); // DISPLAYOFF
    SSD1351_WriteCommand(0xB3); // CLOCKDIV
    SSD1351_WriteCommand(0xF1); // 7:4 = Oscillator Frequency, 3:0 = CLK Div Ratio (A[3:0]+1 = 1..16)
    SSD1351_WriteCommand(0xCA); // MUXRATIO
    {
        uint8_t data[] = { 0x7F }; // 127
        SSD1351_WriteData(data, sizeof(data));
    }
    SSD1351_WriteCommand(0xA0); // SETREMAP
    {
        uint8_t data[] = { 0x74 };
        SSD1351_WriteData(data, sizeof(data));
    }
    SSD1351_WriteCommand(0x15); // SETCOLUMN
    {
        uint8_t data[] = { 0x00, 0x7F };
        SSD1351_WriteData(data, sizeof(data));
    }
    SSD1351_WriteCommand(0x75); // SETROW
    {
        uint8_t data[] = { 0x00, 0x7F };
        SSD1351_WriteData(data, sizeof(data));
    }
    SSD1351_WriteCommand(0xA1); // STARTLINE
    {
        uint8_t data[] = { 0x00 }; // 96 if display height == 96
        SSD1351_WriteData(data, sizeof(data));
    }
    SSD1351_WriteCommand(0xA2); // DISPLAYOFFSET
    {
        uint8_t data[] = { 0x00 };
        SSD1351_WriteData(data, sizeof(data));
    }
    SSD1351_WriteCommand(0xB5); // SETGPIO
    {
        uint8_t data[] = { 0x00 };
        SSD1351_WriteData(data, sizeof(data));
    }
    SSD1351_WriteCommand(0xAB); // FUNCTIONSELECT
    {
        uint8_t data[] = { 0x01 };
        SSD1351_WriteData(data, sizeof(data));
    }
    SSD1351_WriteCommand(0xB1); // PRECHARGE
    {
        uint8_t data[] = { 0x32 };
        SSD1351_WriteData(data, sizeof(data));
    }
    SSD1351_WriteCommand(0xBE); // VCOMH
    {
        uint8_t data[] = { 0x05 };
        SSD1351_WriteData(data, sizeof(data));
    }
    SSD1351_WriteCommand(0xA6); // NORMALDISPLAY
    SSD1351_WriteCommand(0xC1); // CONTRASTABC
    {
        uint8_t data[] = { 0xC8, 0x80, 0xC8 };
        SSD1351_WriteData(data, sizeof(data));
    }
    SSD1351_WriteCommand(0xC7); // CONTRASTMASTER
    {
        uint8_t data[] = { 0x0F };
        SSD1351_WriteData(data, sizeof(data));
    }
    SSD1351_WriteCommand(0xB4); // SETVSL
    {
        uint8_t data[] = { 0xA0, 0xB5, 0x55 };
        SSD1351_WriteData(data, sizeof(data));
    }
    SSD1351_WriteCommand(0xB6); // PRECHARGE2
    {
        uint8_t data[] = { 0x01 };
        SSD1351_WriteData(data, sizeof(data));
    }
    SSD1351_WriteCommand(0xAF); // DISPLAYON

    SSD1351_Unselect();

    //set R/W_Pin LOW
    HAL_GPIO_WritePin(GPIOA, R_W_Pin, GPIO_PIN_RESET);
    //set EN_Pin high
    HAL_GPIO_WritePin(GPIOA, EN_Pin, GPIO_PIN_SET);
}


//write a character
static void SSD1351_WriteChar(uint16_t x, uint16_t y, char ch, FontDef font, uint16_t color, uint16_t bgcolor) {
    uint32_t i, b, j;

    SSD1351_SetAddressWindow(x, y, x+font.width-1, y+font.height-1);

    for(i = 0; i < font.height; i++) {
        b = font.data[(ch - 32) * font.height + i];
        for(j = 0; j < font.width; j++) {
            if((b << j) & 0x8000)  {
                uint8_t data[] = { color >> 8, color & 0xFF };
                SSD1351_WriteData(data, sizeof(data));
            } else {
                uint8_t data[] = { bgcolor >> 8, bgcolor & 0xFF };
                SSD1351_WriteData(data, sizeof(data));
            }
        }
    }
}

//Write a Text on the OLED screen
void SSD1351_WriteString(uint16_t x, uint16_t y, const char* str, FontDef font, uint16_t color, uint16_t bgcolor) {
    SSD1351_Select();

    while(*str) {
        if(x + font.width >= SSD1351_WIDTH) {
            x = 0;
            y += font.height;
            if(y + font.height >= SSD1351_HEIGHT) {
                break;
            }

            if(*str == ' ') {
                // skip spaces in the beginning of the new line
                str++;
                continue;
            }
        }

        SSD1351_WriteChar(x, y, *str, font, color, bgcolor);
        x += font.width;
        str++;
    }

    SSD1351_Unselect();
}

//Fill a Rectangle Screen with a color
void SSD1351_FillRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color) {
    // clipping
    if((x >= SSD1351_WIDTH) || (y >= SSD1351_HEIGHT)) return;
    if((x + w - 1) >= SSD1351_WIDTH) w = SSD1351_WIDTH - x;
    if((y + h - 1) >= SSD1351_HEIGHT) h = SSD1351_HEIGHT - y;

    SSD1351_Select();
    SSD1351_SetAddressWindow(x, y, x+w-1, y+h-1);

    uint8_t data[] = { color >> 8, color & 0xFF };
    HAL_GPIO_WritePin(SSD1351_DC_GPIO_Port, SSD1351_DC_Pin, GPIO_PIN_SET);
    for(y = h; y > 0; y--) {
        for(x = w; x > 0; x--) {
            HAL_SPI_Transmit(&SSD1351_SPI_PORT, data, sizeof(data), HAL_MAX_DELAY);
        }
    }

    SSD1351_Unselect();
}

//Fill Screen with a color
void SSD1351_FillScreen(uint16_t color) {
    SSD1351_FillRectangle(0, 0, SSD1351_WIDTH, SSD1351_HEIGHT, color);
}



//disply the temperature in celsius and fahrenheit on the OLED
void display_output(float result, char buf[], char buf2[]){

    //if temperature under 13°C it will display in blue (cold)
	if(result<15){
		  SSD1351_WriteString(17, 70, buf, Font_11x18, SSD1351_BLUE, SSD1351_BLACK);
		  SSD1351_WriteString(83, 80, "o", Font_7x10, SSD1351_BLUE, SSD1351_BLACK);
		  SSD1351_WriteString(90, 70, "C", Font_11x18, SSD1351_BLUE, SSD1351_BLACK);

		  SSD1351_WriteString(17, 40, buf2, Font_11x18, SSD1351_BLUE, SSD1351_BLACK);
		  SSD1351_WriteString(83, 50, "o", Font_7x10, SSD1351_BLUE, SSD1351_BLACK);
		  SSD1351_WriteString(90, 40, "F", Font_11x18, SSD1351_BLUE, SSD1351_BLACK);

	}

	 //if temperature between 15°C and 28°C it will display in white (normal)
	if(result>=15&&result<28){
		  SSD1351_WriteString(17, 70, buf, Font_11x18, SSD1351_WHITE, SSD1351_BLACK);
		  SSD1351_WriteString(83, 80, "o", Font_7x10, SSD1351_WHITE, SSD1351_BLACK);
		  SSD1351_WriteString(90, 70, "C", Font_11x18, SSD1351_WHITE, SSD1351_BLACK);

		  SSD1351_WriteString(17, 40, buf2, Font_11x18, SSD1351_WHITE, SSD1351_BLACK);
		  SSD1351_WriteString(83, 50, "o", Font_7x10, SSD1351_WHITE, SSD1351_BLACK);
		  SSD1351_WriteString(90, 40, "F", Font_11x18, SSD1351_WHITE, SSD1351_BLACK);

	}

	 //if temperature is over 28°C it will display in red (hot)
	if(result>=28){
		  SSD1351_WriteString(17, 70, buf, Font_11x18, SSD1351_RED, SSD1351_BLACK);
	      SSD1351_WriteString(83, 80, "o", Font_7x10, SSD1351_RED, SSD1351_BLACK);
		  SSD1351_WriteString(90, 70, "C", Font_11x18, SSD1351_RED, SSD1351_BLACK);

		  SSD1351_WriteString(17, 40, buf2, Font_11x18, SSD1351_RED, SSD1351_BLACK);
		  SSD1351_WriteString(83, 50, "o", Font_7x10, SSD1351_RED, SSD1351_BLACK);
		  SSD1351_WriteString(90, 40, "F", Font_11x18, SSD1351_RED, SSD1351_BLACK);
	}

}





