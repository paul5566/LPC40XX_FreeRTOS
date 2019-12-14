#include <stdint.h>

void lcd_send_command(uint8_t *command);
void lcd_send_data(uint8_t data);

void lcd_init(void);
void lcd_font_init(void);
void lcd_test(void);
void lcd_page_mode(void);
void lcd_clear(void);
void lcd_pic(void);
void lcd_print_char_8x8(uint8_t page, uint8_t column_16, uint8_t column_8, uint8_t data);
void lcd_print_string_8x8(uint8_t page, uint8_t * data);
// void lcd_print_char(uint8_t page, uint8_t column_16, uint8_t column_8, uint8_t data);
// void lcd_print_string(uint8_t page, uint8_t * data);
// void lcd_print_char_large(uint8_t page, uint8_t column_16, uint8_t column_8, uint8_t data);
// void lcd_print_string_large(uint8_t page, uint8_t * data);
void lcd_font_printf(void);