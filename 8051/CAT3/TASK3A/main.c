#include <reg51.h>


sfr LED_PORT = 0xB0; // Address of Port 3 (P3)

sbit BTN = P3^4;

// Software delay function for debouncing and pattern timings
void delay_ms(unsigned int ms) {
    unsigned int i, j;
    for (i = 0; i < ms; i++) {
        for (j = 0; j < 120; j++); 
    }
}

//Function to handle active-low LED output while preserving input pin P3.4
void set_leds(unsigned char pattern_nibble) {
    //Invert lower 4 bits for Active-Low LEDs (~pattern)
    //Preserve P3.4 high so it functions properly as an input pin
    LED_PORT = (LED_PORT & 0xF0) | (~pattern_nibble & 0x0F);
}

//Active-low switch debouncing with single-transition registration
//Returns 1 only on a confirmed new press event (falling edge)
unsigned char check_button_pressed(void) {
    if (BTN == 0) {                 // Detect potential press
        delay_ms(20);               // Debounce delay
        if (BTN == 0) {             // Confirm valid press state
            while (BTN == 0);       // Wait for button release (edge detection)
            delay_ms(20);           // Release debounce delay
            return 1;
        }
    }
    return 0;
}

void main(void) {
    unsigned char mode = 0;         // State counter (0 to 4)
    unsigned char step = 0;
    unsigned char count = 0;

    // Initialize Port P3: LEDs OFF (lower 4 bits HIGH), Button pin HIGH (Input)
    LED_PORT = 0xFF;

    while (1) {
        // Poll button press to transition between modes
        if (check_button_pressed()) {
            mode++;
            if (mode > 4) {
                mode = 1;           // Wrap back to Mode 1 after Mode 4
            }
            step = 0;               // Reset sub-state step indices
            count = 0;
        }

        switch (mode) {
            case 0:
                // Initial State: All LEDs OFF
                set_leds(0x00);
                break;

            case 1:
                // Mode 1: Running LED (LED1 -> LED2 -> LED3 -> LED4 -> LED1)
                set_leds(1 << step);
                step = (step + 1) % 4;
                delay_ms(200);
                break;

            case 2:
                // Mode 2: 4-Bit Binary Counter (0000 to 1111)
                set_leds(count & 0x0F);
                count = (count + 1) % 16;
                delay_ms(300);
                break;

            case 3:
                // Mode 3: Alternating Pattern (1010 -> 0101 -> 1010 -> 0101)
                if (step == 0) {
                    set_leds(0x0A); // 1010
                    step = 1;
                } else {
                    set_leds(0x05); // 0101
                    step = 0;
                }
                delay_ms(300);
                break;

            case 4:
                // Mode 4: All LED Flash (1111 -> 0000 -> 1111 -> 0000)
                if (step == 0) {
                    set_leds(0x0F); // 1111
                    step = 1;
                } else {
                    set_leds(0x00); // 0000
                    step = 0;
                }
                delay_ms(300);
                break;

            default:
                mode = 0;
                break;
        }
    }
}