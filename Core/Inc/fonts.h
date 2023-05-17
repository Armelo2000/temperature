/*
 * fonts.h
 *
 *  Created on: Jan 9, 2023
 *      Author: Usman
 *      https://github.com/afiskon/stm32-ssd1351/tree/master/Lib/ssd1351
 */

#ifndef INC_FONTS_H_
#define INC_FONTS_H_


#include <stdint.h>

typedef struct {
    const uint8_t width;
    uint8_t height;
    const uint16_t *data;
} FontDef;


extern FontDef Font_7x10;
extern FontDef Font_11x18;


#endif /* INC_FONTS_H_ */
