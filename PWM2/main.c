// part 2

#include "adc.h"

#include "FreeRTOS.h"
#include "pwm1.h"
#include "queue.h"
#include "task.h"
#include "uart.h"
#include "uart_printf.h"
#include <stdio.h>
#include <stdlib.h>

// This is the queue handle we will need for the xQueue Send/Receive API
QueueHandle_t adc_to_pwm_task_queue;

void adc__enable_burst_mode() {
  LPC_ADC->CR |= (1 << 16);
  LPC_ADC->CR &= ~(7 << 24);
}

void pin_configure_adc_channel_as_io_pin(void) {
  // Function Selection : set 001 for bit 0 - bit 2 for P0.25 (ADC0 channel 2)
  LPC_IOCON->P0_25 |= (0x1 << 0);
  // Clear Bit 7 - Select ADMODE as Analog
  LPC_IOCON->P0_25 &= ~(0x1 << 7);
  LPC_IOCON->P1_31 &= ~(0x1 << 7);
}

void pin_configure_pwm_channel_as_io_pin() {
  LPC_IOCON->P2_0 &= ~(3 << 0);
  LPC_IOCON->P2_0 |= (1 << 0);
}

static void adc_task(void *p) {
  // Reuse the code from Part 1
  adc__initialize();
  // This is the function you need to add to adc.h
  // You can configure burst mode for just the channel you are using
  adc__enable_burst_mode();
  pin_configure_adc_channel_as_io_pin(); // You need to write this function

  int adc_reading = 0; // Note that this 'adc_reading' is not the same variable as the one from adc_task
  uint16_t value = 0;

  while (1) {
    // Implement code to send potentiometer value on the queue
    // a) read ADC input to 'int adc_reading'
    // b) Send to queue: xQueueSend(adc_to_pwm_task_queue, &adc_reading, 0);
    vTaskDelay(100);
    value = adc__get_adc_value(ADC__CHANNEL_5);
    xQueueSend(adc_to_pwm_task_queue, &adc_reading, 0);
    uart_printf__polled(UART__0, "%6u", adc_reading);
    vTaskDelay(100);
    printf("%u\n", value);
  }
}

void pwm_task(void *p) {
  // Reuse the code from Part 0
  pwm1__init_single_edge(1000);
  // We only need to set PWM configuration once, and the HW will drive
  // the GPIO at 1000Hz, and control set its duty cycle to 50%
  pwm1__set_duty_cycle(PWM1__2_0, 20);

  int adc_reading = 0;
  double percent = 0;
  double reading = adc_reading;
  while (1) {
    // Implement code to receive potentiometer value from queue
    if (xQueueReceive(adc_to_pwm_task_queue, &adc_reading, 100)) {
      reading = (adc_reading / 4095) * 100;
      pwm1__set_duty_cycle(PWM1__2_0, reading);
      printf("settig duty cycle\n");
    }

    // We do not need task delay because our queue API will put task to sleep when there is no data in the queue
    // vTaskDelay(100);
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
  uart_puts__polled(UART__0, "PWM LAB2 Start\n");

  adc_to_pwm_task_queue = xQueueCreate(1, sizeof(int));
  xTaskCreate(adc_task, "adc", (512U * 4) / sizeof(void *), NULL, PRIORITY_LOW, NULL);
  xTaskCreate(pwm_task, "pwm", (512U * 4) / sizeof(void *), NULL, PRIORITY_LOW, NULL);
  vTaskStartScheduler();
}
