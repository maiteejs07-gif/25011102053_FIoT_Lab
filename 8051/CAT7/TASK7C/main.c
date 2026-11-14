#include <reg51.h>
#include <intrins.h>

#define LCD_ROWS 2
#define LCD_COLS 16
#define MOVE_CHAR '*'

sbit RS = P3^2;
sbit EN = P3^3;

unsigned char cur_row = 0;
unsigned char cur_col = 0;

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

    lcd_cmd(0x28);
    lcd_cmd(0x0C);
    lcd_cmd(0x01);
    delay_ms(2);
    lcd_cmd(0x06);
}

void lcd_goto(unsigned char row, unsigned char col) {
    unsigned char address;
    if (row >= LCD_ROWS || col >= LCD_COLS) return;

    address = (row == 0) ? col : (0x40 + col);
    lcd_cmd(0x80 | address);
}

int keypad_scan(void) {
    static const int keypad[4][4] = {
        {'1', '2', '3', 'A'},
        {'4', '5', '6', 'B'},
        {'7', '8', '9', 'C'},
        {'*', '0', '#', 'D'}
    };
    static const unsigned char rowPin[4] = {7, 6, 5, 4};
    static const unsigned char colPin[4] = {3, 2, 1, 0};
    int row, col;

    for (row = 0; row < 4; row++) {
        P2 |= 0xF0;
        P2 &= ~(1 << rowPin[row]);

        for (col = 0; col < 4; col++) {
            if ((P2 & (1 << colPin[col])) == 0) {
                return keypad[row][col];
            }
        }
    }
    return -1;
}

void move_char(char direction) {
    // Clear old position
    lcd_goto(cur_row, cur_col);
    lcd_data(' ');

    // Update coordinates based on key
    switch (direction) {
        case '6': // Move Right
            if (cur_col < LCD_COLS - 1) cur_col++;
            break;
        case '4': // Move Left
            if (cur_col > 0) cur_col--;
            break;
        case '8': // Move Down
            if (cur_row < LCD_ROWS - 1) cur_row++;
            break;
        case '2': // Move Up
            if (cur_row > 0) cur_row--;
            break;
    }

    // Draw at new position
    lcd_goto(cur_row, cur_col);
    lcd_data(MOVE_CHAR);
}

void main(void) {
    int key;

    lcd_init();
    
    // Draw initial character at top-left
    lcd_goto(cur_row, cur_col);
    lcd_data(MOVE_CHAR);

    while (1) {
        key = keypad_scan();
        if (key == '2' || key == '4' || key == '6' || key == '8') {
            move_char((char)key);
            delay_ms(250); // Button press debounce delay
        }
    }
}