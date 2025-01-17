/*Absolute function*/
#include <stdlib.h>
/*ADC LIBRARY*/
#include "adc.h"
#include "gpio.h"


static const int right_stand = 2400;
static const int left_stand = 2200;
static const int up_stand = 2400;
static const int down_stand = 2200;
static const int normal_stand = 2300;
/*enum type*/
enum StickerDir { NONE, LEFT, RIGHT, UP, DOWN };


enum StickerDir dir_check(uint16_t x_val, uint16_t y_val) {
  /*case that Y's diff is greater than X*/
  if (y_val == 0) {
    if (x_val > right_stand) {
      return RIGHT;
    } else if (x_val < left_stand) {
      return LEFT;
    } else
      return NONE;
  } else {   /*case that X's diff us graeter than Y */
    if (y_val > up_stand) {
      return UP;
    } else if (y_val < down_stand) {
      return DOWN;
    } else
      return NONE;
  }
}


/*
 * 	Intialization and Set UP for StickerDir adc_task
 *	(1)ADC INTIALIZATION
 *		adc__initialize();
 *	(2) chanel2 and chanzel5 set up
 * 		pin_configure_adc_channel_as_io_pin();
 */
enum  StickerDir adc_task(void *p) {
  int x_val = 0;
  int y_val = 0;
  int check_val_x = 0;
  int check_val_y = 0;
  enum StickerDir dir_status;

  /*get direction from joysticker with ADC*/
  x_val = adc__get_adc_value(ADC__CHANNEL_5);
  y_val = adc__get_adc_value(ADC__CHANNEL_2);
  check_val_x = abs(x_val - normal_stand);
  check_val_y = abs(y_val - normal_stand);

  if (check_val_x > check_val_y)
    dir_status = dir_check(x_val,0);
  else
    dir_status = dir_check(0,y_val);
  switch (dir_status) {
    case 0:
      return NONE;
      break;
    case 1:
      return LEFT;
      break;
    case 2:
      return RIGHT;
      break;
    case 3:
      return UP;
      break;
    case 4:
      return DOWN;
      break;
    default:
	  return NONE;
	  //break;
    }
}
