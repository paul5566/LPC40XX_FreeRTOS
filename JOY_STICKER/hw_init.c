#include "adc.h"
#include "gpio.h"
#include "uart.h"
#include <stdio.h>
/*pin configuration*/
#include "lpc40xx.h"

/*
 * Place hardware initialization code here
 */
void hw_init(void) {

  //////////////////////////////////////////////////////////////////////////
  //																	  //
  //				ADC Intialization                                     //
  //				set pin_configure_adc_channel_as_io_pin				  //
  // 				ADC Chanel 4,5					                      //
  //																      //
  //////////////////////////////////////////////////////////////////////////

  /*	ADC Intialization from adc.h*/
  adc__initialize();
  /*
   * Function Selection : set 110 for bit 0-2 for P1.30(ADC0 chanel 4)
   * Function Selection : set 110 for bit 0-2 for P1.31(ADC0 chanel 5)
   * gpio__construct_with_function(GPIO__PORT_0, 25, GPIO__FUNCTION_1);
   */
  LPC_IOCON->P1_30 |= (0x3 << 0);
  LPC_IOCON->P1_31 |= (0x3 << 0);
  /* Clear Bit 7 - Select ADMODE as Analog forADC chanel 4,5*/
  LPC_IOCON->P1_30 &= ~(0x1 << 7);
  LPC_IOCON->P1_31 &= ~(0x1 << 7);

  //////////////////////////////////////////////////////////////////////////
  //																	  //
  //				 										              //
  //				set gpio pin as the input				              //
  //				GPIO Chanel P1_20,P1_23					              //
  //																      //
  //////////////////////////////////////////////////////////////////////////
  /* set as input P1_20 && P1_23 */
  LPC_GPIO1->DIR &= ~(1 << 20);
  LPC_GPIO1->DIR &= ~(1 << 23);
  /* set as PULL DOWN */
  LPC_IOCON->P1_20 |= (1 << 3);
  LPC_IOCON->P1_20 &= ~(1 << 4);
  LPC_IOCON->P1_23 |= (1 << 3);
  LPC_IOCON->P1_23 &= ~(1 << 4);
}
