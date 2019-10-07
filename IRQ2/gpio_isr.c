#include "gpio_isr.h"
#include "uart.h"
#include "uart_printf.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// Note: You may want another separate array for falling vs. rising edge callbacks
static function_pointer_t switch_falling_callbacks[32];
static function_pointer_t switch_rising_callbacks[32];

/*
 *  using CLR0 registers to clear the interrupt of port0
 */
static void clear_register_clr0(uint32_t pin) {
  LPC_GPIOINT->IO0IntClr |= (1 << pin);
  uart_puts__polled(UART__0, "\n----clear the CLR register----\n");
}

void gpio0__attach_interrupt(uint32_t pin, gpio_interrupt_e interrupt_type, function_pointer_t callback) {
  // 1) Store the callback based on the pin at switch0_callbacks
  if (interrupt_type == GPIO_INTR__FALLING_EDGE) {
    switch_falling_callbacks[pin] = callback;
  } else {
    switch_rising_callbacks[pin] = callback;
  }
  // 2) Configure the SW's pin for rising or falling edge
  if (interrupt_type == GPIO_INTR__FALLING_EDGE) {
    LPC_GPIOINT->IO0IntEnF |= (1 << pin);
  } else {
    LPC_GPIOINT->IO0IntEnR |= (1 << pin);
  }
}

// We wrote some of the implementation for you
void gpio0__interrupt_dispatcher(void) {
  int irq_pin = 87;
  function_pointer_t attached_user_handler;
  bool FallIrq_sate = false;
  // Check which pin generated the interrupt
  for (int i = 0; i < 32; i++) {
    uint32_t Fall_check = (LPC_GPIOINT->IO0IntStatF) >> i;

    if (1 == (Fall_check & 1)) {
      irq_pin = i;
      FallIrq_sate = true;
      break;
    }
  }

  if (false == FallIrq_sate) {
    for (int i = 0; i < 32; i++) {
      uint32_t Rise_check = (LPC_GPIOINT->IO0IntStatR) >> i;

      if (1 == (Rise_check & 1)) {
        irq_pin = i;
        break;
      }
    }
  }

  if (true == FallIrq_sate) {
    attached_user_handler = switch_falling_callbacks[irq_pin];
  } else {
    attached_user_handler = switch_rising_callbacks[irq_pin];
  }
  // Invoke the user registered callback, and then clear the interrupt
  attached_user_handler();
  clear_register_clr0(irq_pin);
}
