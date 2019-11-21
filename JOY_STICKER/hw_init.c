#include <stdio.h>

#include "hw_init.h"
#include "gpio.h"
#include "uart.h"
#include "adc.h"
/*pin configuration*/
#include "lpc40xx.h"


/**
 *  * Place hardware initialization code here
 *   */
void hw_init(void) {
//////////////////////////////////////////////////////////////////////////
//																		//
//				ADC Intialization 										//
//				set pin_configure_adc_channel_as_io_pin					//
//				Chanel Available 2,4,5					                //
//																		//
//////////////////////////////////////////////////////////////////////////
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

