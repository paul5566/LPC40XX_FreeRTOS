// ADC PWM part 1

#include "FreeRTOS.h"
#include "adc.h"
#include "lpc40xx.h"
#include "task.h"
#include "uart.h"
#include "uart_printf.h"
#include <stdio.h>
#include <stdlib.h>

void adc__enable_burst_mode() {
  // burst_mode: page 802
  LPC_SC->PCONP |= (1 << 12);
  LPC_ADC->CR = (1 << 21);
  LPC_ADC->CR |= (1 << 16);
  LPC_ADC->CR &= ~(7 << 24);

  // LPC_ADC->STAT |= (1 << 0);

  // 12.4MHz max
  const uint32_t max_adc_clock = (12 * 1000UL * 1000UL);
  const uint32_t adc_clock = clock__get_peripheral_clock_hz();
  for (uint32_t divider = 2; divider < 255; divider += 2) {
    if ((adc_clock / divider) < max_adc_clock) {
      LPC_ADC->CR |= (divider << 8);
      break;
    }
  }
}

void pin_configure_adc_channel_as_io_pin() {
  // ADC[5] = 011;
  // LPC_IOCON->P1_31 &= ~(3 << 0);
  LPC_IOCON->P1_31 |= (3 << 0);
  // gpio__construct_with_function(1, 31, 3);
}

uint16_t adc__get_channel_reading_with_burst_mode(adc_channel_e ADC__CHANNEL) {
  LPC_ADC->CR &= ~((1 << 24) | (0xFF));
  LPC_ADC->CR |= (1 << 5);
  uint16_t result = 1;
  result = (LPC_ADC->GDR >> 4) & 0x0FFF;
  return result;
}

void adc_task(void *p) {
  adc__initialize();

  // This is the function you need to add to adc.h
  // You can configure burst mode for just the channel you are using
  adc__enable_burst_mode();

  // Configure a pin, such as P1.31 with FUNC 011 to route this pin as ADC channel 5
  // You can use gpio__construct_with_function() API from gpio.h
  pin_configure_adc_channel_as_io_pin(); // You need to write this function

  uint16_t value = 0;
  char *string;

  while (1) {
    // Get the ADC reading using adc__get_adc_value() and print it
    // printf("%u\n", adc__get_channel_reading_with_burst_mode(ADC__CHANNEL_5)); // You need to write this function
    vTaskDelay(100);
    value = adc__get_adc_value(ADC__CHANNEL_5);
    vTaskDelay(100);
    printf("%u\n", value);
    /*
			value = adc__get_channel_reading_with_burst_mode(ADC__CHANNEL_5);
			vTaskDelay(100);
			if (value != 0) {
				uart_puts__polled(UART__0, "Not zero\n"); // cost??
				sprintf(string, "%d", value);
			uart_puts__polled(UART__0, string); // cost??
    */
    // sprintf(string, "%d", value);
    // uart_puts__polled(UART__0, string); // cost??
    // uart_puts__polled(UART__0, "end of the loop\n");
  }
}

static void uart0_init(void) {
  uart__init(UART__0, clock__get_peripheral_clock_hz(), 115200);
  QueueHandle_t tx_queue = xQueueCreate(128, sizeof(char));
  QueueHandle_t rx_queue = xQueueCreate(32, sizeof(char));
  uart__enable_queues(UART__0, tx_queue, rx_queue);
}

void main(void) {
  uart0_init();
  uart_puts__polled(UART__0, "PWM LAB1 Start\n");

  xTaskCreate(adc_task, "adc", (512U * 4) / sizeof(void *), NULL, PRIORITY_LOW, NULL);
  vTaskStartScheduler();
}
