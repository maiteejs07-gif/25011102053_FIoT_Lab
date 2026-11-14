#include <reg51.h>

unsigned char rx_data;

void serial_interrupt(void) interrupt 4 {
    if (RI) {
        RI = 0;            // Clear receive interrupt flag
        rx_data = SBUF;    // Read received character
        SBUF = rx_data;    // Echo character back to transmitter
    }
    
    if (TI) {
        TI = 0;            // Clear transmit interrupt flag
    }
}

void UART_Init(void) {
    SCON = 0x50;  // Mode 1: 8-bit UART, receiver enabled
    TMOD |= 0x20; // Timer 1: Mode 2 (8-bit auto-reload)
    TH1 = 0xFD;   // 9600 baud rate at 11.0592 MHz
    TR1 = 1;      // Start Timer 1
    ES = 1;       // Enable Serial interrupt
    EA = 1;       // Enable Global interrupt
}

void main(void) {
    UART_Init();

    while (1) {
        // Main loop remains free for other processing while interrupts handle echo
    }
}