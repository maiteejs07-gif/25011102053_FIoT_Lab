#include <reg51.h>
#include <intrins.h>

sbit RS = P3^2;
sbit EN = P3^3;

void delay_ms(unsigned int ms) {
    unsigned int i, j;
    for (i = 0; i < ms; i++) {
        for (j = 0; j < 160; j++);
    }
}

// Send 4-bit nibble to LCD
void lcd_nibble(unsigned char nibble, bit rs_val) {
    RS = rs_val;
    P3 = (P3 & 0x0F) | ((nibble & 0x0F) << 4);
    EN = 1;
    _nop_();
    _nop_();
    EN = 0;
}

// Send command to LCD
void lcd_cmd(unsigned char val) {
    lcd_nibble((val >> 4) & 0x0F, 0);
    lcd_nibble(val & 0x0F, 0);
}

// Send data byte to LCD
void lcd_data(unsigned char val) {
    lcd_nibble((val >> 4) & 0x0F, 1);
    lcd_nibble(val & 0x0F, 1);
}

// Initialize LCD in 4-bit, 2-line mode
void lcd_init(void) {
    P3 = 0x00;
    RS = 0;
    EN = 0;
    delay_ms(20);

    // Initialization sequence for 4-bit mode
    lcd_nibble(0x03, 0);
    delay_ms(5);
    lcd_nibble(0x03, 0);
    delay_ms(1);
    lcd_nibble(0x03, 0);
    delay_ms(1);
    lcd_nibble(0x02, 0); // Select 4-bit mode
    delay_ms(1);

    lcd_cmd(0x28); // 4-bit mode, 2 lines, 5x8 font
    lcd_cmd(0x0C); // Display ON, Cursor OFF
    lcd_cmd(0x01); // Clear display
    delay_ms(2);
    lcd_cmd(0x06); // Entry mode: increment cursor
}

void lcd_str(char *str) {
    while (*str) {
        lcd_data(*str++);
    }
}

void main(void) {
    lcd_init();

    // Row 1 Display
    lcd_cmd(0x80); // Move to Row 0, Col 0
    lcd_str("Row 1: Task 7A");

    // Row 2 Display
    lcd_cmd(0xC0); // Move to Row 1, Col 0
    lcd_str("Hello World!");

    while (1);
}