#include <stdio.h>
#include <stdlib.h>

/*ADC LIBRARY*/
#include "adc.h"

#include "FreeRTOS.h"
#include "gpio.h"
#include "lpc40xx.h"
#include "task.h"

static const int right_stand = 2400;
static const int left_stand = 2200;
static const int up_stand = 2400;
static const int down_stand = 2200;
static const int normal_stand = 2300;
/*enum type*/
enum StickerDir { NONE, LEFT, RIGHT, UP, DOWN };

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

void pin_configure_adc_channel_as_io_pin(void) {
  /*
   * Function Selection : set 001 for bit 0-2 for P0.25 (ADC0 channel 2)
   * Function Selection : set 110 for bit 0-2 for P1.30(ADC0 chanel 4)
   * Function Selection : set 110 for bit 0-2 for P1.31(ADC0 chanel 5)
   * gpio__construct_with_function(GPIO__PORT_0, 25, GPIO__FUNCTION_1);
   */

  LPC_IOCON->P0_25 |= (0x1 << 0);
  LPC_IOCON->P1_30 |= (0x3 << 0);
  LPC_IOCON->P1_31 |= (0x3 << 0);
  /* Clear Bit 7 - Select ADMODE as Analog forADC chanel 2,4,5*/
  LPC_IOCON->P0_25 &= ~(0x1 << 7);
  LPC_IOCON->P1_30 &= ~(0x1 << 7);
  LPC_IOCON->P1_31 &= ~(0x1 << 7);
}

void adc_task(void *p) {
  int x_val = 0;
  int y_val = 0;
  int check_val_x = 0;
  int check_val_y = 0;

  enum StickerDir dir_status;
  // Intialization
  adc__initialize();
  // chanel2 and chanzel5 set up
  pin_configure_adc_channel_as_io_pin();

  while (1) {

    x_val = adc__get_adc_value(ADC__CHANNEL_5);
    y_val = adc__get_adc_value(ADC__CHANNEL_4);
    check_val_x = abs(x_val - normal_stand);
    check_val_y = abs(y_val - normal_stand);

    if (check_val_x > check_val_y)
      dir_status = dir_check(x_val, 0);
    else
      dir_status = dir_check(0, y_val);

    vTaskDelay(200);
    printf("val of x:%u\t val of y :%u\n", x_val, y_val);
    /*return as the enum_type*/
    switch (dir_status) {
    case 0:
      printf("NONE\n");
      // return NONE;
      break;
    case 1:
      printf("LEFT\n");
      // return LEFT;
      break;
    case 2:
      printf("RIGHT\n");
      // return RIGHT;
      break;
    case 3:
      printf("up\n");
      // return UP;
      break;
    case 4:
      printf("DOWN\n");
      // return DOWN;
      break;
    default:
      printf("NONE\n");
      // return NONE;
      // break;
    }
    vTaskDelay(200);
  }
}
int main(void) {
  // printf("JOY_STICKER\n");
  xTaskCreate(adc_task, "adc", (1024U * 4) / sizeof(void *), NULL, PRIORITY_LOW, NULL);
  vTaskStartScheduler();
}
