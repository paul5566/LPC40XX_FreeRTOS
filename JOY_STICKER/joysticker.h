#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

/*gpio*/
#include "lpc40xx.h"

/*ADC LIBRARY*/
#include "adc.h"

/*enum type*/
enum StickerDir { NONE, LEFT, RIGHT, UP, DOWN };
/*dir_check*/
enum StickerDir dir_check(uint16_t x_val, uint16_t y_val);
/*get joy sticker direction*/
enum StickerDir get_joysticker_dir(void);
/*Get Button Status*/
// static bool gpio1__get_level(uint8_t pin_num);

bool slection(void);
bool cancle(void);
