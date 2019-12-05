#include "joysticker.h"

static const int right_stand = 2400;
static const int left_stand = 2200;
static const int up_stand = 2400;
static const int down_stand = 2200;
static const int normal_stand = 2300;

enum StickerDir dir_check(uint16_t x_val, uint16_t y_val) {
  if (y_val == 0) {
    if (x_val > right_stand) {
      return RIGHT;
    } else if (x_val < left_stand) {
      return LEFT;
    } else
      return NONE;
  } else {
    if (y_val > up_stand) {
      return UP;
    } else if (y_val < down_stand) {
      return DOWN;
    } else
      return NONE;
  }
}
/*Get Button Status*/
static bool gpio1__get_level(uint8_t pin_num) { return LPC_GPIO1->PIN & (1 << pin_num); }

bool slection(void) {
  bool stat_selection;
  stat_selection = gpio1__get_level(20);
  if (stat_selection == 0)
    return false;
  else
    return true;
}

bool cancle(void) {
  bool stat_cancle;
  stat_cancle = gpio1__get_level(23);
  if (stat_cancle == 0)
    return false;
  else
    return true;
}

enum StickerDir get_joysticker_dir(void) {
  int x_val = 0;
  int y_val = 0;
  int check_val_x = 0;
  int check_val_y = 0;

  enum StickerDir dir_status;

  x_val = adc__get_adc_value(ADC__CHANNEL_5);
  y_val = adc__get_adc_value(ADC__CHANNEL_4);
  check_val_x = abs(x_val - normal_stand);
  check_val_y = abs(y_val - normal_stand);

  if (check_val_x > check_val_y)
    dir_status = dir_check(x_val, 0);
  else
    dir_status = dir_check(0, y_val);

  printf("val of x:%u\t val of y :%u\n", x_val, y_val);
  /*return as the enum_type*/
  switch (dir_status) {
  case 0:
    printf("NONE\n");
    return NONE;
    break;
  case 1:
    printf("LEFT\n");
    return LEFT;
    break;
  case 2:
    printf("RIGHT\n");
    return RIGHT;
    break;
  case 3:
    printf("UP\n");
    return UP;
    break;
  case 4:
    printf("DOWN\n");
    return DOWN;
    break;
  default:
    printf("NONE\n");
    return NONE;
    break;
  }
}
