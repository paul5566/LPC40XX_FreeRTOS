#include <stdint.h>
#include "ssp2.h"


void ssp2__init(uint32_t max_clock_mhz) {
  // Refer to LPC User manual and setup the register bits correctly
  // a) Power on Peripheral
  LPC_PCONP |= 1<<20;//


// Refer to LPC User manual and setup the register bits correctly
    LPC_SC->PCLKSEL = 1;
    LPC_SSP2->CPSR = 4;
		/* reset 0 don't trust the default*/
    LPC_IOCON->P1_0 &= ~(7<<0);
    LPC_IOCON->P1_1 &= ~(7<<0);
    LPC_IOCON->P1_4 &= ~(7<<0);
    LPC_IOCON->P1_10 &= ~(7<<0);

    LPC_IOCON->P1_0 |= (4<<0);
    LPC_IOCON->P1_1 |= (4<<0);
    LPC_IOCON->P1_4 |= (4<<0);
		/*????Doubt????*/
    LPC_IOCON->P1_10 |= (0<<0); //pin select???
    // b) Setup control registers CR0 and CR1
    // c) Setup prescalar register to be <= max_clock_mhz
}

uint8_t ssp2__exchange_byte(uint8_t data_out) {
  // Configure the Data register(DR) to send and receive data by checking the status register
	LPC_SSP2->DR = byte_to_transmit;

  while (LPC_SSP2->SR & (1 << 4)) {
    ; // Wait until SSP is busy
  }
  return (uint8_t)(LPC_SSP2->DR & 0xFF);
}



