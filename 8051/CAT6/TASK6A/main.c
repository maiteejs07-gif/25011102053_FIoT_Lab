#include <reg51.h>

void UART_Init(void) {
    SCON = 0x50;  // 8-bit UART, REN enabled
    TMOD |= 0x20; // Timer 1, Mode 2 (8-bit auto-reload)
    TH1 = 0xFD;   // 9600 baud rate at 11.0592 MHz
    TR1 = 1;      // Start Timer 1
}

void UART_TxChar(char ch) {
    SBUF = ch;
    while (!TI);   // Wait until transmission is complete
    TI = 0;        // Clear transmit interrupt flag
}

void UART_TxString(char *str) {
    while (*str) {
        UART_TxChar(*str++);
    }
}

void main(void) {
    UART_Init();

    while (1) {
        UART_TxString("Hello from 8051\r\n");
        
        // Optional delay between transmissions so terminal isn't overwhelmed
        {
            unsigned int i, j;
            for(i = 0; i < 500; i++)
                for(j = 0; j < 1200; j++);
        }
    }
}