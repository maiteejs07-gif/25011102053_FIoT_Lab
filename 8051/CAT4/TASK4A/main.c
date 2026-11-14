#include <reg51.h>

//Function definitions 
void delayms(unsigned int duration);
int keypad_scan(void);
int keypad_wait(unsigned int n);
void display_digit(int value);
void led_control(unsigned char key);
void running_led(void);
void alternate_led(void);
void binary_counter(void);
void blink_all(void);
void led_pattern(unsigned char pattern);

//Global variables
int mode = -1;

//Main
void main(void)
{
    int key;

    // Initial blank state
    P0 = 0x00;      // 7-segment display OFF
    P1 = 0x00;      // LEDs OFF

    while(1)
    {
        display_digit(mode);

        switch(mode)
        {
            case 1:
							   led_control(mode);
						     break;
            case 2:
							   led_control(mode);
						     break;
            case 3:
							   led_control(mode);
						     break;
            case 4:
                led_control(mode);
                break;

            case 5:
                running_led();
                break;

            case 6:
                alternate_led();
                break;

            case 7:
                binary_counter();
                break;

            case 8:
                blink_all();
                break;

            case 9:
                led_pattern(0x00); // All LEDs OFF
                break;

            case 0:
                // Reset System State
                P0 = 0x00;
                P1 = 0x00;
                mode = -1;
                break;

            default:
                key = keypad_scan();
                if(key != -1)
                {
                    mode = key;
                }
                break;
        }
    }
}

//Delay 
void delayms(unsigned int duration)
{
    unsigned int i, j;
    for(i = 0; i < duration; i++)
    {
        for(j = 0; j < 350; j++);
    }
}

//Keypad scan function
int keypad_scan(void)
{
    static const int keypad[4][4] =
    {
        {1, 2, 3, 4},
        {5, 6, 7, 8},
        {9, 0, -1, -1},
        {-1, -1, -1, -1}
    };

    static const unsigned char rowPin[4] = {7, 6, 5, 4};
    static const unsigned char colPin[4] = {3, 2, 1, 0};
    int row, col;

    for(row = 0; row < 4; row++)
    {
        // 1. Set all rows HIGH and enable pull-ups on columns
        P2 = 0xFF;

        // 2. Drive current row LOW
        P2 &= ~(1 << rowPin[row]);

        // 3. Check column inputs
        for(col = 0; col < 4; col++)
        {
            if((P2 & (1 << colPin[col])) == 0)
            {
                return keypad[row][col];
            }
        }
    }

    return -1;
}

//Keypad wait 
int keypad_wait(unsigned int n)
{
    unsigned int i;
    int key;

    for(i = 0; i < n; i++)
    {
        key = keypad_scan();
        if(key != -1)
        {
            return key;
        }
        delayms(10);
    }

    return -1;
}

//Seven segment display 
void display_digit(int value)
{
    static const unsigned char digit[10] =
    {
        0x3F,   // 0
        0x06,   // 1
        0x5B,   // 2
        0x4F,   // 3
        0x66,   // 4
        0x6D,   // 5
        0x7D,   // 6
        0x07,   // 7
        0x7F,   // 8
        0x6F    // 9
    };

    if(value >= 0 && value <= 9)
    {
        P0 = digit[value];
    }
    else
    {
        P0 = 0x00;
    }
}

//LED Mode functions
void led_control(unsigned char key)
{
    int new_key;
    P1 = (1 << (key - 1));

    while(1)
    {
        new_key = keypad_wait(1);
        if(new_key != -1)
        {
            mode = new_key;
            return;
        }
    }
}

void running_led(void)
{
    int i;
    int key;

    while(1)
    {
        for(i = 1; i <= 4; i++)
        {
            P1 = (1 << (i - 1));
            key = keypad_wait(50);
            if(key != -1)
            {
                mode = key;
                return;
            }
        }
    }
}

void alternate_led(void)
{
    static const unsigned char pattern[2] = {0x05, 0x0A}; // 0101 and 1010 binary
    int i;
    int key;

    while(1)
    {
        for(i = 0; i < 2; i++)
        {
            P1 = pattern[i];
            key = keypad_wait(50);
            if(key != -1)
            {
                mode = key;
                return;
            }
        }
    }
}

void binary_counter(void)
{
    int i;
    int key;

    while(1)
    {
        for(i = 0; i <= 15; i++)
        {
            P1 = i;
            key = keypad_wait(50);
            if(key != -1)
            {
                mode = key;
                return;
            }
        }
    }
}

void blink_all(void)
{
    static const unsigned char pattern[2] = {0x0F, 0x00};
    int i;
    int key;

    while(1)
    {
        for(i = 0; i < 2; i++)
        {
            P1 = pattern[i];
            key = keypad_wait(50);
            if(key != -1)
            {
                mode = key;
                return;
            }
        }
    }
}

void led_pattern(unsigned char pattern)
{
    int key;
    P1 = pattern;

    while(1)
    {
        key = keypad_wait(1);
        if(key != -1)
        {
            mode = key;
            return;
        }
    }
}