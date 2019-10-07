#include "FreeRTOS.h"
#include "lpc40xx.h"
#include "pwm1.h"
#include "task.h"

void pin_configure_pwm_channel_as_io_pin() {
  // P2_0 = PWM1[1]
  //*(0x4002C100) &= ~(3 << 0);
  //*(0x4002C100) |= (1 << 0);
  LPC_IOCON->P2_0 &= ~(3 << 0);
  LPC_IOCON->P2_0 |= (1 << 0);
  // gpio__set_function(2, 0, 0b001); // set [2:0] of P2_0 = PWM1[1]; the same as above

  // LPC_IOCON->P2_0 |= (1 << 4); // enable pull up resistor
  // LPC_IOCON->P2_0 |= (1 << 10); // set open drain
}

void pwm_task(void *p) {
  pwm1__init_single_edge(1000);

  // Locate a GPIO pin that a PWM channel will control
  // You can use gpio__construct_with_function() API from gpio.h
  pin_configure_pwm_channel_as_io_pin();

  // We only need to set PWM configuration once, and the HW will drive
  // the GPIO at 1000Hz, and control set its duty cycle to 50%
  pwm1__set_duty_cycle(PWM1__2_0, 50);

  // Continue to vary the duty cycle in the loop
  uint8_t percent = 0;
  while (1) {
    pwm1__set_duty_cycle(PWM1__2_0, percent);

    if (++percent > 100) {
      percent = 0;
    }
    vTaskDelay(100);
  }
}

void main(void) {
  xTaskCreate(pwm_task, "pwm", (512U * 4) / sizeof(void *), NULL, PRIORITY_LOW, NULL);
  vTaskStartScheduler();
}
