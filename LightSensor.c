/*
This code configures the onboard light sensor on the MSP430fr2355 as an input for the final project.
*/

#include <msp430.h>

volatile long light, lightRaw; // Variables for storing light sensor data

void ConfigureAdc_light(); // Function to configure the ADC for the light sensor

void main(void)
{
    WDTCTL = WDTPW + WDTHOLD; // Stop watchdog timer
    __delay_cycles(5); // Wait for ADC Ref to settle
    __delay_cycles(400); // Additional settling delay

    ConfigureAdc_light(); // Initialize ADC for the light sensor

    while(1){
        ADCCTL0 |= ADCENC | ADCSC; // Enable and start ADC conversion

        while ((ADCIFG & ADCIFG0) == 0); // Wait for ADC conversion to complete
        light = ADCMEM0; // Read the converted data into the light variable
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
