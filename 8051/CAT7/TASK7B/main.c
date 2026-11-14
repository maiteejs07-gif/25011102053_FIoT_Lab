#include <reg51.h>
#include <intrins.h>

#define LCD_ROWS 2
#define LCD_COLS 16

sbit RS = P3^2;
sbit EN = P3^3;

void delay_ms(unsigned int ms);
void lcd_nibble(unsigned char nibble, bit rs_val);
void lcd_cmd(unsigned char val);
void lcd_data(unsigned char val);
void lcd_init(void);
void lcd_str(char *str);
void lcd_goto(unsigned char row, unsigned char col);
void lcd_puts(unsigned char row, unsigned char col, char *str);
void lcd_print_number(unsigned int value);


void delay_ms(unsigned int ms) {
    unsigned int i, j;
    for (i = 0; i < ms; i++) {
        for (j = 0; j < 160; j++);
    }
}


void lcd_nibble(unsigned char nibble, bit rs_val) {
    RS = rs_val;
    P3 = (P3 & 0x0F) | ((nibble & 0x0F) << 4);
    EN = 1;
    _nop_();
    _nop_();
    EN = 0;
}

void lcd_cmd(unsigned char val) {
    lcd_nibble((val >> 4) & 0x0F, 0);
    lcd_nibble(val & 0x0F, 0);
}

void lcd_data(unsigned char val) {
    lcd_nibble((val >> 4) & 0x0F, 1);
    lcd_nibble(val & 0x0F, 1);
}

void lcd_init(void) {
    P3 = 0x00;
    RS = 0;
    EN = 0;
    delay_ms(20);

    lcd_nibble(0x03, 0);
    delay_ms(5);
    lcd_nibble(0x03, 0);
    delay_ms(1);
    lcd_nibble(0x03, 0);
    delay_ms(1);
    lcd_nibble(0x02, 0);
    delay_ms(1);

    lcd_cmd(0x28); // 4-bit mode, 2 lines, 5x8 font
    lcd_cmd(0x0C); // Display ON, Cursor OFF
    lcd_cmd(0x01); // Clear display
    delay_ms(2);
    lcd_cmd(0x06); // Entry mode set
}



//Display string on LCD
void lcd_str(char *str) {
    while (*str) {
        lcd_data(*str++);
    }
}

//Position cursor
void lcd_goto(unsigned char row, unsigned char col) {
    unsigned char address;
    if (row >= LCD_ROWS || col >= LCD_COLS) return;

    address = (row == 0) ? col : (0x40 + col);
    lcd_cmd(0x80 | address);
}

//Position cursor and display string
void lcd_puts(unsigned char row, unsigned char col, char *str) {
    lcd_goto(row, col);
    lcd_str(str);
}

//Print dynamic numerical values
void lcd_print_number(unsigned int value) {
    unsigned char digits[5];
    signed char i = 0;

    if (value == 0) {
        lcd_data('0');
        return;
    }

    while (value > 0) {
        digits[i++] = value % 10;
        value /= 10;
    }

    while (i > 0) {
        i--;
        lcd_data(digits[i] + '0');
    }
}

void main(void) {
    unsigned int counter = 0;

    lcd_init();
    
    // Static label at top row
    lcd_puts(0, 0, "Counter Value:");

    while (1) {
        // Dynamic value update at bottom row
        lcd_goto(1, 0);
        lcd_print_number(counter);
        lcd_str("     "); // Blank spaces to clear old trailing digits
        
        counter++;
        delay_ms(500);
    }
}