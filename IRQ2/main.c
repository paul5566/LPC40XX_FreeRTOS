#include "FreeRTOS.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include "FreeRTOS.h"
#include "task.h"

#include "board_io.h"
#include "delay.h"
#include "gpio.h"
#include "uart.h"

#include "gpio_isr.h"
#include "semphr.h"

#include "board_io.h"
#include "uart_printf.h"
#include <lpc_peripherals.h>
/*
 *  GLOBAL　VARIABLE
 */

/*	led		*/
static gpio_s led0 = {.port_number = gpio__port_2, .pin_number = 3};
static gpio_s led1 = {.port_number = gpio__port_1, .pin_number = 26};
static gpio_s led2 = {.port_number = gpio__port_1, .pin_number = 24};
static gpio_s led3 = {.port_number = gpio__port_1, .pin_number = 18};
/*	switch	*/
static gpio_s SW_2 = {.port_number = 0, .pin_number = 30};
static gpio_s SW_3 = {.port_number = 0, .pin_number = 29};
/*	Semaphore Handle	*/
SemaphoreHandle_t SW_2_pressed_signal, SW_3_pressed_signal;

/*
 *	Function & Task
 */

static void switchIrq_Hndlr(void) { gpio0__interrupt_dispatcher(); }

void SW_2_TaskSemaphore(void) { xSemaphoreGiveFromISR(SW_2_pressed_signal, NULL); }
void SW_3_TaskSemaphore(void) { xSemaphoreGiveFromISR(SW_3_pressed_signal, NULL); }

static void BLINK_LED(void *params) {
  const gpio_s led_blink = *((gpio_s *)params);
  uint8_t count = 0;
  while (count < 10) {
    gpio__toggle(led_blink);
    vTaskDelay(250);
    count++;
  }
}

void SW_2_ISR(void *parameter) {
  while (1) {
    // Use xSemaphoreTake with forever delay and blink an LED when you get the signal
    if (xSemaphoreTake(SW_2_pressed_signal, portMAX_DELAY - 1)) {
      /* We were able to obtain the semaphore and can now access the
      shared resource. */
      uart_puts__polled(UART__0, "\n========\nINTERRUPT_SW2\n=========\n");
      gpio__set_as_output(led2); // set as the output for LED2
      // ISR->Toggle
      BLINK_LED(&led2);
    }
  }
}

void SW_3_ISR(void *parameter) {
  while (1) {
    // Use xSemaphoreTake with forever delay and blink an LED when you get the signal
    if (xSemaphoreTake(SW_3_pressed_signal, portMAX_DELAY - 1)) {
      uart_puts__polled(UART__0, "\n========\nINTERRUPT_SW3\n=========\n");
      gpio__set_as_output(led3); // set as the output for LED3
      // ISR->Toggle
      BLINK_LED(&led3);
    }
  }
}

static void uart_task(void *params) {
  TickType_t previous_tick = 0;
  while (true) {
    // This loop will repeat every 500 ticks, even if the logic below takes variable amount of ticks
    vTaskDelayUntil(&previous_tick, 1000);
    printf("ticking\n");
  }
}
// blink_on_startup
static void blink_on_startup(gpio_s gpio, int blinks) {
  const int toggles = (blinks);
  for (int i = 0; i < toggles; i++) {
    delay__ms(250);
    gpio__toggle(gpio);
  }
}

static void uart0_init(void) {
  // Note: PIN functions are initialized by board_io__initialize() for P0.2(Tx) and P0.3(Rx)
  uart__init(UART__0, clock__get_peripheral_clock_hz(), 115200);

  // Make UART more efficient by backing it with RTOS queues (optional but highly recommended with RTOS)
  QueueHandle_t tx_queue = xQueueCreate(128, sizeof(char));
  QueueHandle_t rx_queue = xQueueCreate(32, sizeof(char));
  uart__enable_queues(UART__0, tx_queue, rx_queue);
}

int main(void) {
  /// UART initialization is required in order to use <stdio.h> puts, printf() etc; @see system_calls.c
  uart0_init();
  uart_puts__polled(UART__0, "\n=========\nInterrupt LAB2\n========\n");

  // Blink LED0 as the hint for beginning;
  blink_on_startup(led0, 2);
  blink_on_startup(led1, 2);
  blink_on_startup(led2, 2);
  blink_on_startup(led3, 2);

  /*
   * Enable Pull Down for Switch 2 and Switch 3
   *
   * Set up the SW configuration
   *
   */
  LPC_IOCON->P1_19 |= (1 << 3);
  LPC_IOCON->P1_19 &= ~(1 << 4);
  LPC_IOCON->P1_15 |= (1 << 3);
  LPC_IOCON->P1_15 &= ~(1 << 4);
  /*set SW2 and SW3 as input*/
  gpio__set_as_input(SW_2);
  gpio__set_as_input(SW_3);

  /*
   * Created the signal of the binary semaphore
   */
  SW_2_pressed_signal = xSemaphoreCreateBinary();
  SW_3_pressed_signal = xSemaphoreCreateBinary();
  /*
   * "Enable the Interrupt"
   *
   *	void gpio0__attach_interrupt(
   *		(1)uint32_t pin
   *		(2)gpio_interrupt_e interrupt_type
   *		(3)function_pointer_t
   *		(4)callback)
   */
  lpc_peripheral__enable_interrupt(LPC_PERIPHERAL__GPIO, switchIrq_Hndlr);
  gpio0__attach_interrupt(30, GPIO_INTR__FALLING_EDGE, SW_2_TaskSemaphore);
  gpio0__attach_interrupt(29, GPIO_INTR__RISING_EDGE, SW_3_TaskSemaphore);

  NVIC_EnableIRQ(GPIO_IRQn); // Enable interrupt gate for the GPIO

  xTaskCreate(uart_task, "uart", (512U * 8) / sizeof(void *), NULL, PRIORITY_LOW, NULL);
  xTaskCreate(SW_2_ISR, "ISR_SW_2", (512U * 4) / sizeof(void *), NULL, PRIORITY_LOW, NULL);
  xTaskCreate(SW_3_ISR, "ISR_SW_3", (512U * 4) / sizeof(void *), NULL, PRIORITY_LOW, NULL);

  puts("Starting Interrupt2 RTOS\n");
  vTaskStartScheduler();
}
