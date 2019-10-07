#include <stdio.h>

#include "FreeRTOS.h"
#include "task.h"

#include "board_io.h"
#include "delay.h"
#include "gpio.h"



//global
int main(void) {
  // Construct the LEDs and blink a startup sequence
  led0 = board_io__get_led0();
  led1 = board_io__get_led1();
  blink_on_startup(led1, 2);

  puts("\n--------\nStartup");
  xTaskCreate(blink_task, "led0", (512U / sizeof(void *)), (void *)&led0, PRIORITY_LOW, NULL);
  xTaskCreate(blink_task, "led1", (512U / sizeof(void *)), (void *)&led1, PRIORITY_LOW, NULL);

  // printf() takes more stack space, size this tasks' stack higher
  xTaskCreate(uart_task, "uart", (512U * 8) / sizeof(void *), NULL, PRIORITY_LOW, NULL);

  puts("Starting RTOS");
  vTaskStartScheduler(); // This function never returns unless RTOS scheduler runs out of memory and fails

  return 0;
}


