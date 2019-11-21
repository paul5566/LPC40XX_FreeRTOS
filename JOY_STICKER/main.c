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

/*enum type*/
enum StickerDir { NONE = 1, LEFT, RIGHT, UP, DOWN };

enum StickerDir dir_y_check(uint16_t y) {
  if (y > up_stand) {
    // printf("UP!\n");
    return UP;
  } else if (y < down_stand) {
    // printf("DOWN!\n");
    return DOWN;
  } else
    return NONE;
}

enum StickerDir dir_x_check(uint16_t x) {
  if (x > right_stand) {
    // printf("RIGHT\t");
    return RIGHT;
  } else if (x < left_stand) {
    // printf("LEFT\t");
    return LEFT;
  } else
    return NONE;
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
  uint16_t x_val = 0;
  uint16_t y_val = 0;
  enum StickerDir dir_status;
  // Intialization
  adc__initialize();
  // chanel2 and chanzel5 set up
  pin_configure_adc_channel_as_io_pin();

  while (1) {
    x_val = adc__get_adc_value(ADC__CHANNEL_5);
    dir_status = dir_x_check(x_val);
    // printf("X direction status is %d\n", dir_status);
    if (dir_status == NONE) {
      y_val = adc__get_adc_value(ADC__CHANNEL_4);
      dir_status = dir_y_check(y_val);
      // printf("Y direction status is %d\n", dir_status);
    }

    vTaskDelay(200);

    printf("val of x:%u\t val of y :%u\n", x_val, y_val);

    switch (dir_status) {
    case 1:
      printf("NONE\n");
      break;
    case 2: // code to be executed if n = 1;
      printf("LEFT\n");
      break;
    case 3:
      printf("RIGHT\n");
      break;
    case 4:
      printf("UP\n");
      break;
    case 5:
      printf("DOWN\n");
      break;
    default:
      break;
    }
    vTaskDelay(200);
  }
}

int main(void) {
  printf("JOY_STICKER\n");
  xTaskCreate(adc_task, "adc", (1024U * 4) / sizeof(void *), NULL, PRIORITY_LOW, NULL);
  vTaskStartScheduler();
}
