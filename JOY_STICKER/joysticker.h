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
/*get joy sticker direction*/
enum StickerDir joysticker__getdir(void);
/*Get Button Status*/
bool selection(void);
bool cancle(void);
