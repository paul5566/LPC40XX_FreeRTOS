#include "FreeRTOS.h"
#include "hw_init.h"
#include "joysticker.h"
#include "task.h"

void joysticker_task(void *p) {
  // enum StickerDir direction_status;
  bool Select_Status;
  bool Cancle_Status;
  while (1) {
    joysticker__getdir();
    Select_Status = selection();
    Cancle_Status = cancle();
    if (Select_Status == 1)
      printf("Select\n");
    /*Testing*/
    if (Cancle_Status == 1)
      printf("CANCLE\n");
    vTaskDelay(500);
  }
}

int main(void) {
  printf("JOY_STICKER\n");
  hw_init();
  xTaskCreate(joysticker_task, "sticker", (1024U * 4) / sizeof(void *), NULL, PRIORITY_LOW, NULL);
  vTaskStartScheduler();
}
