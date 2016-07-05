/*
 * File: candle.c
 *
 * Created: 09.05.2016 
 * Author: Mar.lux
 *
 *
 * The candle waits for nightfall (darkness > NightlLevel), 
 * The candle then burns for some time (CandleMinutes minutes),
 * Then, the device waits for the next day and night to repeat
 *
 *
 * Calibration needed!
 * The parameters NightLevel and DayLevel change with selection of LED and voltage
 * Some software engineering might be needed to get to the right values for your specific application
 *
 * 
 * Power consumption (3V):
 *
 * Waiting for day or night: < 1 uA 
 * Candle burning (no LED): 5 uA
 * Candle burning (with yellow LED): 2.3 mA
 *
 * Created on 15 May 2016, 21:19
 */


// CONFIG1
#pragma config FOSC = INTOSC // Oscillator Selection (INTOSC oscillator: I/O function on CLKIN pin)
#pragma config WDTE = SWDTEN // Watchdog Timer Enable (WDT controlled by the SWDTEN bit in the WDTCON register)
#pragma config PWRTE = ON    // Power-up Timer Enable (PWRT enabled)
#pragma config MCLRE = ON // MCLR Pin Function Select (MCLR/VPP pin function is MCLR)
#pragma config CP = OFF // Flash Program Memory Code Protection (Program memory code protection is disabled)
#pragma config CPD = OFF // Data Memory Code Protection (Data memory code protection is disabled)
#pragma config BOREN = OFF // Brown-out Reset Enable (Brown-out Reset disabled)
#pragma config CLKOUTEN = OFF // Clock Out Enable (CLKOUT function is disabled. I/O or oscillator function on the CLKOUT pin)
#pragma config IESO = ON // Internal/External Switchover (Internal/External Switchover mode is enabled)
#pragma config FCMEN = ON // Fail-Safe Clock Monitor Enable (Fail-Safe Clock Monitor is enabled)

// CONFIG2
#pragma config WRT = OFF // Flash Memory Self-Write Protection (Write protection off)
#pragma config PLLEN = OFF // PLL Enable (4x PLL disabled)
#pragma config STVREN = ON // Stack Overflow/Underflow Reset Enable (Stack Overflow or Underflow will cause a Reset)
#pragma config BORV = LO // Brown-out Reset Voltage Selection (Brown-out Reset Voltage (Vbor), low trip point selected.)
#pragma config LVP = OFF // Low-Voltage Programming Enable (Low-voltage programming enabled)

#define LED TRISA5
#define LEDR RA5

#define GND TRISA4
#define GNDR RA4
#define GNDWPU WPUA4
#define GNDANS ANSA4


#define CandleMinutes 180 // in minutes, how long the candle burns
#define NightLevel 30
#define DayLevel 4
#define TriggerCount 5   // number of day/night measurements to detect day/night


#include <xc.h>
#include <stdint.h>

uint8_t isDarkerThan(uint8_t level);
volatile char tcounter = 0;

void main(void) {
    uint16_t RND_Num = 0x3333; // Current random number
    uint8_t mode = 1; // 1: day, 2: candle, 0: night
    uint8_t counter = 0;

    CCP1SEL = 1; // Timer2 PMW output on RA5
        
    LED = 0; // 1. Set LED pin as an output to enable PWM output
    GND = 0;
    GNDANS = 0; // define GND as digital

    WDTCON = 0b00010011; // WDT prescaler to 0.25s, enable WDT               
    LEDR = 1;
    SLEEP();
    LEDR = 0;
    SLEEP();
    
    // slow down
    OSCCON = 0b00000000; // 31kHz p53
    // IRCF    -xxxx---


    while (1) {
        //////////////////////////////////////////////////////////////////////////
        // Wait for nightfall
        //////////////////////////////////////////////////////////////////////////
        while (mode < 2) 
        {
            counter = 0;
            while (counter < TriggerCount) {
                WDTCON = 0b00011111; // WDT prescaler to 30 second, enable WDT             
                SLEEP(); // sleep until Watchdog timer overflows

                if (mode) {
                    if (isDarkerThan(NightLevel)) counter++;
                    else if (counter) counter--;
                } else {
                    if (!isDarkerThan(DayLevel)) counter++;
                    else if (counter) counter--;
                }
            }
            mode++;
        }

        WDTCON = 0; // disable WDT               


        //////////////////////////////////////////////////////////////////////////
        // Let the candle burn
        //////////////////////////////////////////////////////////////////////////
        // Set up PWM module
        CCP1CON = 0b00001100; // 3./4. PWM single outout mode, active high
        // 5. a) ignored since smooth startup is not needed

        TMR2ON = 1; // 5. b/c) Timer 2 (PWM) ON
        //  T2CON = 0b00001100; // Turn Timer 2 on, set postscaler
        // PSbits:-xxxx---

        PR2 = 127; // overflow compare match at 127 to reduce cycle time
        
        TMR2IF = 0;
        TMR2IE = 1; // enable timer 2 interrupt
        PEIE = 1; // Enables All Unmasked Peripheral Interrupts
        GIE = 1; // Global Interrupt Enabled

        uint8_t minutes = CandleMinutes;
        int8_t state = 1;
        uint8_t flickerstrength = 4;
        uint8_t counter2 = 0;

        while (minutes) {
            while (tcounter); // wait for tcounter to roll over
            RND_Num = 2053 * RND_Num + 13849; // get new random number
            //RND_Num = (RND_Num << 11) + (RND_Num << 2) + RND_Num + 13849; // get new random number
            // 2053 = 2048 + 4 + 1
            if (++counter2 == 0) // once per minute
            {
                minutes--;
                flickerstrength = 1 + (RND_Num >> 14); // 1 ... 4
            }

            uint8_t r = (RND_Num >> 13); // 0 ... 7
            state = -state;
            //CCPR1L = 80 + 2 * flickerstrength * (7 - r) * state; // 80 +/- 56 -> 24 .. 136
            CCPR1L = 50 + flickerstrength * (7 - r) * state; // half of it... 50 +/- 28 -> 
            //tcounter = ((r * r + 2 * r + 4)); // 4, 7, 12, 19, 28, 39, 52, 67
            tcounter = ((r * r) >> 1) + r + 2; 
        }
                
        CCP1CON = 0; 
        TMR2ON = 0; 
        TMR2IE = 0; // disable timer 2 interrupt
        PEIE = 0; // Disable All Unmasked Peripheral Interrupts
        TMR2IF = 0;      
        
        mode = 0; // night
    }

    return;
}

void interrupt timer2_ISR(void) {
    tcounter--; // increments at about 244 Hz
    TMR2IF = 0;
}

uint8_t isDarkerThan(uint8_t level) {
    WDTCON = 0b00001001; // WDT prescaler to 16 ms, enable WDT               
    
    // Reverse charge LED
    LEDR = 0; // pull LED pin low
    GNDR = 1; // pull GND high
    SLEEP();

    //Setup measurement
    GND = 1; // set GND as input
    GNDWPU = 0; // turn off GND pull-up

    while (level--) SLEEP();
    uint8_t isset = GNDR;
    GNDR = 0;
    GND = 0; // set GND as output
    return isset;
}