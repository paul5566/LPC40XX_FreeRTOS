#include "FreeRTOS.h"
#include "task.h"
#include "ssp2.h"

// TODO: Implement Adesto flash memory CS signal as a GPIO driver
void adesto_cs(void);
void adesto_ds(void);

// TODO: Study the Adesto flash 'Manufacturer and Device ID' section
typedef struct {
  uint8_t manufacturer_id;
  uint8_t device_id_1;
  uint8_t device_id_2;
  uint8_t extended_device_id;
} adesto_flash_id_s;



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

    LPC_IOCON->P1_0 |= (4<<0);//SSP2_SCK
    LPC_IOCON->P1_1 |= (4<<0);//SSP2_MOSI
    LPC_IOCON->P1_4 |= (4<<0);//SSP2_MISO
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

// TODO: Implement the code to read Adesto flash memory signature
// TODO: Create struct of type 'adesto_flash_id_s' and return it
adesto_flash_id_s ssp2__adesto_read_signature(void) {
  //adesto_flash_id_s data = { 0 };
    adesto_cs();
  {
    // Send opcode and read bytes
    // TODO: Populate members of the 'adesto_flash_id_s' struct(Device ID)
		//TABLE 484?? ->0110
 		LPC_SSP2->CR1 |= 0x6;
    d[0] = spi_transfer(0x40); // TODO: Find what to send to read Adesto flash signature
    d[1] = spi_transfer(0x00);
  }
  adesto_ds();
  //return data;
}

void spi_task(void *p) {
  uint32_t spi_clock_mhz = 24;
  ssp2__init(spi_clock_mhz);
  // From the LPC schematics pdf, find the pin numbers connected to flash memory
  // Read table 84 from LPC User Manual and configure PIN functions for SPI2 pins
  // You can use gpio__construct_with_function() API from gpio.h
  //
  // Note: Configure only SCK2, MOSI2, MISO2.
  // CS will be a GPIO output pin(configure and setup direction)
  todo_configure_your_ssp2_pin_functions();

  while (1) {
    adesto_flash_id_s id = ssp2__adesto_read_signature();
    // TODO: printf the members of the 'adesto_flash_id_s' struct
    vTaskDelay(500);
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


void main(void) {
	/// UART initialization is required in order to use <stdio.h> puts, printf() etc; @see system_calls.c
  uart0_init();
  uart_puts__polled(UART__0, "\n=========\nInterrupt LAB2\n========\n");
  xTaskCreate(spi_task, "SPI2", (512U * 8) / sizeof(void *), NULL, PRIORITY_LOW, NULL);
  vTaskStartScheduler();
}
