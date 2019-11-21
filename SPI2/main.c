#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"

#include "board_io.h"
#include "delay.h"
#include "gpio.h"
#include "uart.h"
#include "delay.h"
#include "lpc40xx.h"
#include "ssp2_lab.h"
#include "semphr.h"


// TODO: Study the Adesto flash 'Manufacturer and Device ID' section
typedef struct {
    uint8_t manufacturer_id;
    uint8_t device_id_1;
    uint8_t device_id_2;
    uint8_t extended_device_id;
} adesto_flash_id_s;



//GLOBAL VARIABLE
SemaphoreHandle_t spi_bus_mutex;


// TODO: Implement Adesto flash memory CS signal as a GPIO driver
/* Chip Select Externel Flash(Low Active)*/
static void adesto_cs(void){
    LPC_IOCON->P1_10 |= (0<<0);
    LPC_GPIO1->DIR |= (1<<10);
	//low active-> CLR
    LPC_GPIO1->CLR |= (1<<10);
}
/* Chip Select Externel Flash(Low Active)*/
static void adesto_ds(void){
    LPC_IOCON->P1_10 |= (0<<0);
    LPC_GPIO1->DIR |= (1<<10);
    LPC_GPIO1->SET |= (1<<10);
}



adesto_read_status(void) {
    uint8_t st = LPC_SSP2->SR;
   	//return st;
}


// TODO: Implement the code to read Adesto flash memory signature
// TODO: Create struct of type 'adesto_flash_id_s' and return it
adesto_flash_id_s ssp2__adesto_read_signature(void) {
    adesto_flash_id_s data = { 0 };
    
    adesto_cs();
    {
        adesto_read_status();
        // Send opcode and read bytes
        // TODO: Populate members of the 'adesto_flash_id_s' struct
        ssp2__exchange_byte_lab(0x9F);
        data.manufacturer_id = ssp2__exchange_byte_lab(0xFF);
        data.device_id_1 = ssp2__exchange_byte_lab(0xFF);
        data.device_id_2 = ssp2__exchange_byte_lab(0xFF);
        data.extended_device_id = ssp2__exchange_byte_lab(0xFF);
    }
    adesto_ds();

    return data;
}


// TODO: Implement the code to read Adesto flash memory signature
// TODO: Create struct of type 'adesto_flash_id_s' and return it
/*
     *  (A)	Send opcode("0x9f")
     *  (B)	Read bytes
     *	(C) Assign the information to Data stored in the 
     *		members of the 'adesto_flash_id_s' struct
     */



void spi_id_verification_task(char *task_name) {
	adesto_flash_id_s ID = {0};
	while (1) {
        if(xSemaphoreTake(spi_bus_mutex, 1000)) {
            // Use Guarded Resource
            ID = ssp2__adesto_read_signature();
            // Give Semaphore back:
            xSemaphoreGive(spi_bus_mutex);
        }
    // When we read a manufacturer ID we do not expect, we will kill this task
    	if (ID.manufacturer_id != 0x1F) {
      		fprintf(stderr, "Manufacturer ID read failure\n");
      		vTaskSuspend(NULL); // Kill this task
    	}
		else	{
			vTaskDelay(250);
			printf("====================\n");
        	printf("%s: ID READ CORRECTLY!\n",task_name);
			printf("====================\n");
			vTaskDelay(250);
		}
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
    // UART initialization is required in order to use <stdio.h> puts, printf() etc; @see system_calls.c
    uart0_init();
	uint32_t spi_clock_mhz = 24;
    ssp2__init_lab(spi_clock_mhz);


    puts("\n--------\nStartup");
	spi_bus_mutex = xSemaphoreCreateMutex();

    // printf() takes more stack space, size this tasks' stack higher
    xTaskCreate(spi_id_verification_task, "spi_1", (512U * 8) / sizeof(void *), "task_1", PRIORITY_LOW, NULL);
    xTaskCreate(spi_id_verification_task, "spi_2", (512U * 8) / sizeof(void *), "task_2", PRIORITY_LOW, NULL);

    puts("Starting the FREE_RTOS");
    vTaskStartScheduler();

    return 0;
}
