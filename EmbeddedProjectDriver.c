/*
This code is the application driver for controlling a DC motor with a light sensor.
*/

#include <msp430.h>

volatile long light, lightRaw; // Variables for storing light sensor data
volatile unsigned int degrees = 0; // Variable for storing motor rotation degrees

void ConfigureAdc_light(); // Function to configure the ADC for the light sensor
void GPIOInit(); // Function to initialize GPIO pins
void TimerInit(); // Function to initialize the timer

void main(void) {
    WDTCTL = WDTPW + WDTHOLD; // Stop watchdog timer
    __delay_cycles(5); // Wait for ADC Ref to settle
    __delay_cycles(400); // Additional settling delay

    GPIOInit(); // Initialize GPIO pins
    TimerInit(); // Initialize timer
    ConfigureAdc_light(); // Initialize ADC for the light sensor

    while(1) {
        ADCCTL0 |= ADCENC | ADCSC; // Enable and start ADC conversion

        while ((ADCIFG & ADCIFG0) == 0); // Wait for ADC conversion to complete
        light = ADCMEM0; // Read the converted data into the light variable

        if (light >= 4095) { // Check if the light value is at the maximum
            if(TB3CCR1 >= 1100) // Adjust the lower limit
                TB3CCR1 -= 100; // Decrease duty cycle by 10%
            else
                TB3CCR1 = 1000; // Set duty cycle to lower limit
        }
        else {
            if(TB3CCR1 <= 1900) // Adjust the upper limit
                TB3CCR1 += 100; // Increase duty cycle by 10%
            else
                TB3CCR1 = 2000; // Set duty cycle to upper limit
        }
    }
}

// Configure ADC for the light sensor
void ConfigureAdc_light(){
    ADCCTL0 |= ADCSHT_8 | ADCON; // Enable ADC, set long sample period
    ADCCTL1 |= ADCSHP | ADCCONSEQ_2; // Set software trigger, single channel/conversion
    ADCCTL2 &= ~ADCRES; // Clear ADCRES in ADCCTL
    ADCCTL2 |= ADCRES_2 | ADCSR; // Set 12-bit conversion results
    ADCMCTL0 |= ADCSREF_0 | ADCINCH_3; // Set ADC input channel A3 for light sensor
    ADCIE |= ADCIE0; // Enable ADC interrupt
}

void TimerInit()
{
    // Configure Timer_B3
    TB3CTL = TBSSEL_2 | MC__UP | TBCLR; // Set SMCLK, up mode, clear timer
    TB3CCTL1 = OUTMOD_7;                // Set CCR1 to reset/set
    TB3CCR0 = 20000;                    // Set frequency to 20kHz
    TB3CCR1 = 1500;                     // Set initial duty cycle
}

void GPIOInit()
{
    // Configure pin P6.0 for the motor
    P6OUT &= ~BIT0;     // Set P6.0 to 0
    P6DIR |= BIT0;      // Set P6.0 as output
    P6SEL0 |= BIT0;     // Select PWM
    P6SEL1 &= ~BIT0;    // Clear bit
}
