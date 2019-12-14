#include "lcd.h"
#include "i2c.h"
#include <string.h>
#include "font8x8_basic.h"

void lcd_send_command(uint8_t *command){
  i2c__write_slave_data(I2C__2, 0x78, &command[1], &command[2], command[0]-2);
}

void lcd_send_data(uint8_t data){
  uint8_t *c = &data;
  i2c__write_slave_data(I2C__2, 0x78, 0x40, c, 1);
}

void lcd_init(void){
  #define array_size 39
  uint8_t array[array_size]={array_size,
                              0x80, 0xAE, // Display OFF
                              0x80, 0xA8, 0x80, 0x3F, // Multiplex Ratio 16-64
                              0x80, 0xD3, 0x80, 0x00, // Display Offset 0-63
                              0x80, 0xA6,
                              0x80, 0x40, // Display Start Line 40-7F
                              0x80, 0xA1, // Segment Re-map, column address 0 is mapped to SEG0
                              0x80, 0xC8, // COM Output Scan Direction, normal mode (RESET) Scan from COM0 to COM[N –1]
                              0x80, 0xDA, 0x80, 0x12, // COM Pins Hardware Configuration, Sequential COM pin configuration, Enable COM Left/Right remap
                              0x80, 0x81, 0x80, 0x7F, // Contrast Control 00-FF
                              0x80, 0xA4, // Entire Display ON A4/A5
                              0x80, 0xD5, 0x80, 0x80, // Display Clock Divide Ratio/ Oscillator Frequency
                              0x80, 0x8D, 0x80, 0x14,
                              0x80, 0xAF  // Display ON
                            };
  lcd_send_command(array);
}

void lcd_page_mode(void){
  #define array_size 5
  uint8_t array[array_size] = {array_size, 0x80, 0x20, 0x80, 0x02};
  lcd_send_command(array);
}

void lcd_pic(void){
  #define array_size 8+4+16
  uint8_t array[array_size] = {array_size,
                                    0x80, 0x20, 0x80, 0x02,
                                    0x80, 0xB0,
                                    0x80, 0x00,
                                    0x80, 0x10,
                                    0x40,
                                    0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0,
                                    0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F
                                    // 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
                                  };
  for(int page=0; page<8; page++){
    array[2+4]=0xB0+page;
    for(int column_16=0; column_16<8; column_16++){
      array[6+4]=0x10+column_16;
      lcd_send_command(array);
    }
  }
}

void lcd_clear(void){
  #define array_size 8+4+8
  uint8_t array[array_size] = {array_size,
                                    0x80, 0x20, 0x80, 0x02,
                                    0x80, 0xB0,
                                    0x80, 0x00,
                                    0x80, 0x10,
                                    0x40,
                                    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
                                    // 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
                                  };
  for(int page=0; page<8; page++){
    array[2+4]=0xB0+page;
    for(int column_16=0; column_16<8; column_16++){
      array[6+4]=0x10+column_16;
      for(int column_8=0; column_8<16; column_8+=8){
        array[4+4]=0x00+column_8;
        lcd_send_command(array);
      }
    }
  }
}

void lcd_test(void){
  #define array_size 12+20
  uint8_t array[array_size] = {array_size,
                                    0x80, 0x20, 0x80, 0x02, // Memory Addressing Mode, Page Addressing Mode
                                    0x80, 0xB7,
                                    0x80, 0x03,
                                    0x80, 0x10,
                                    // 0x80, 0x21, 0x80, 0x00, 0x80, 0x7F,
                                    0x40,
                                    0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F,
                                    0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F
                                  };
  lcd_send_command(array);
}

void lcd_print_char_8x8(uint8_t page, uint8_t column_16, uint8_t column_8, uint8_t data){
  #define array_size 8+4+8
  uint8_t array[array_size] = {array_size,
                                0x80, 0x20, 0x80, 0x02,
                                0x80, 0xB1,
                                0x80, 0x00,
                                0x80, 0x10,
                                0x40,
                                0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
                              };
  array[6]=0xB0+page;
  array[8]=0x00+column_8;
  array[10]=0x10+column_16;
  for(int i=0; i<8; i++){
    array[array_size-8+i]=lcd_font8x8_basic[data][i];
    // printf("%x\n", lcd_font8x8_basic[data][i]);
  }
  lcd_send_command(array);
}

void lcd_print_string_8x8(uint8_t page, uint8_t * data){
  int length = strlen(data);
  int i=0;
  uint8_t column_16 = 0;
  uint8_t column_8 = 0;
  while(data[i]!='\0' && i<16 && i<length){
    lcd_print_char_8x8(page, column_16, column_8, data[i]);
    if(column_8==8){
      column_8=0;
      column_16++;
    }
    else{
      column_8=8;
    }
    i++;
  }
}

void lcd_font_init(void){
  uint8_t transform[8][8]={0};
  uint8_t column_value = 0;
  for(int i=0;i<128;i++){
    for(int j=0;j<8;j++){
      for(int k=0;k<8;k++){
        transform[j][k] = (font8x8_basic[i][j]>>k) & 1;
      }
    }
    for(int j=0;j<8;j++){
      for(int k=0;k<8;k++){
        column_value |= transform[k][j] << k;
      }
      lcd_font8x8_basic[i][j] = column_value;
      column_value = 0;
    }
  }
}

void lcd_font_printf(void){
  for(int i=0;i<128;i++){
    for (int j=0;j<8;j++){
      printf("%x, ", lcd_font8x8_basic[i][j]);
    }
    printf("\n");
  }
}

// uint8_t lcd_font[10][4]={0};
// void lcd_font_init(void){
//   uint8_t ascii_0 = 0x30;
//   lcd_font['0'-ascii_0][0] = 0x30;
//   lcd_font['0'-ascii_0][1] = 0x48;
//   lcd_font['0'-ascii_0][2] = 0x30;
//   lcd_font['0'-ascii_0][3] = 0x00;
// }

// void lcd_print_char(uint8_t page, uint8_t column_16, uint8_t column_8, uint8_t data){
//   #define array_size 8+4+4
//   uint8_t array[array_size] = {array_size,
//                                 0x80, 0x20, 0x80, 0x02,
//                                 0x80, 0xB1,
//                                 0x80, 0x00,
//                                 0x80, 0x10,
//                                 0x40,
//                                 0xFF, 0xFF, 0xFF, 0xFF
//                               };
//   array[6]=0xB0+page;
//   array[8]=0x00+column_8;
//   array[10]=0x10+column_16;
//   for(int i=0; i<4; i++){
//     array[array_size-4+i]=lcd_font[data-0x30][i];
//     printf("%x\n", lcd_font[data-0x30][i]);
//   }
//   lcd_send_command(array);
// }

// void lcd_print_string(uint8_t page, uint8_t * data){
//   int length = strlen(data);
//   int i=0;
//   uint8_t column_16 = 0;
//   uint8_t column_8 = 0;
//   while(data[i]!='\0' && i<32 && i<length){
//     lcd_print_char(page, column_16, column_8, data[i]);
//     if(column_8==12){
//       column_8=0;
//       column_16++;
//     }
//     else{
//       column_8+=4;
//     }
//     i++;
//   }
// }

// // 4x7
// // uint8_t char_0[8] = {0x0C, 0x12, 0x0C, 0x00};
// uint8_t char_0_0[8] = {0x00, 0x00, 0xC0, 0xC0, 0x00, 0x00, 0x00, 0x00};
// uint8_t char_0_1[8] = {0x0F, 0x0F, 0x30, 0x30, 0x0F, 0x0F, 0x00, 0x00};

// void lcd_print_char_large(uint8_t page, uint8_t column_16, uint8_t column_8, uint8_t data){
//   #define array_size 8+4+8
//   uint8_t array[array_size] = {array_size,
//                                 0x80, 0x20, 0x80, 0x02,
//                                 0x80, 0xB1,
//                                 0x80, 0x00,
//                                 0x80, 0x10,
//                                 0x40,
//                                 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
//                               };
//   array[8]=0x00+column_8;
//   array[10]=0x10+column_16;
//   for(int i=0; i<8; i++){
//     array[array_size-8+i]=char_0_0[i];
//   }
//   array[6]=0xB0+page;
//   lcd_send_command(array);
//   for(int i=0; i<8; i++){
//     array[array_size-8+i]=char_0_1[i];
//   }
//   array[6]=0xB1+page;
//   lcd_send_command(array);
//   // for(int i=0; i<8; i++){
//   //   array[array_size-8+i]=char_0_0[i];
//   // }
//   // for(int i=0; i<8;i+=2){
//   //   array[6]=0xB0+i;
//   //   lcd_send_command(array);
//   // }
//   // for(int i=0; i<8; i++){
//   //   array[array_size-8+i]=char_0_1[i];
//   // }
//   // for(int i=1; i<8;i+=2){
//   //   array[6]=0xB0+i;
//   //   lcd_send_command(array);
//   // }
// }

// void lcd_print_string_large(uint8_t page, uint8_t * data){
//   int length = strlen(data);
//   int i=0;
//   uint8_t column_16 = 0;
//   uint8_t column_8 = 0;
//   while(data[i]!='\0' && i<16 && i<length){
//     lcd_print_char_large(page, column_16, column_8, data[i]);
//     if(column_8==0){
//       column_8=8;
//     }
//     else{
//       column_8=0;
//       column_16++;
//     }
//     i++;
//   }
// }
