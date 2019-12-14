// @file gpio_isr.c
#include "gpio_isr.h"
#include "uart.h"
#include "uart_printf.h"
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

// Note: You may want another separate array for falling vs. rising edge callbacks
static function_pointer_t gpio0_callbacks_falling[32];
static function_pointer_t gpio0_callbacks_rising[32];

typedef void (*function_pointer_t)(void);

void gpio0__attach_interrupt(uint32_t pin, gpio_interrupt_e interrupt_type, function_pointer_t callback) {
    // 1) Store the callback based on the pin at gpio0_callbacks
	if(interrupt_type == GPIO_INTR__FALLING_EDGE){
		gpio0_callbacks_falling[pin] = callback;
	}else{
		gpio0_callbacks_rising[pin] = callback;
	}
    // 2) Configure GPIO 0 pin for rising or falling edge
	if(interrupt_type == GPIO_INTR__FALLING_EDGE){
		LPC_GPIOINT->IO0IntEnF |= (1 << pin);
	}else{
		LPC_GPIOINT->IO0IntEnR |= (1 << pin);
	}
}

int logic_that_you_will_write() {
	int pin = 0;
	if (LPC_GPIOINT->IO0IntStatF) {
		while (pin < 32) {
			if(((LPC_GPIOINT->IO0IntStatF >> pin) & 1) == 1)
				break;
			pin++;
		}
	}
	else if (LPC_GPIOINT->IO0IntStatR) {
		while (pin < 32) {
			if(((LPC_GPIOINT->IO0IntStatR >> pin) & 1) == 1)
				break;
			pin++;
		}
	}
	return pin;
}

void clear_pin_interrupt(int pin) {
	LPC_GPIOINT->IO0IntClr |= (1 << pin);
}

// We wrote some of the implementation for you
void gpio0__interrupt_dispatcher(void) {

    // Check which pin generated the interrupt
    const int pin_that_generated_interrupt = logic_that_you_will_write();
	function_pointer_t attached_user_handler;
    if (LPC_GPIOINT->IO0IntStatF) {
    	attached_user_handler = gpio0_callbacks_falling[pin_that_generated_interrupt];
    }
    else if (LPC_GPIOINT->IO0IntStatR) {
	    attached_user_handler = gpio0_callbacks_rising[pin_that_generated_interrupt];
    }

    // Invoke the user registered callback, and then clear the interrupt
	attached_user_handler();
	clear_pin_interrupt(pin_that_generated_interrupt);
}