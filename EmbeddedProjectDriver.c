/*
This section is the final software of our final project.
*/

#include <msp430.h>

volatile long light, lightRaw;
volatile unsigned int degrees = 0;

void ConfigureAdc_light();
void GPIOInit();
void TimerInit();

void main(void) {
    WDTCTL = WDTPW + WDTHOLD; // Stop watchdog timer
    __delay_cycles(5); // Wait for ADC Ref to settle
    __delay_cycles(400);

    GPIOInit();
    TimerInit();
    ConfigureAdc_light();

    while(1) {
        ADCCTL0 |=ADCENC | ADCSC; //Enable and start conversion

        while ((ADCIFG &ADCIFG0)==0);
        light = ADCMEM0; // read the converted data into a variable

        if (light >= 4095) {
            if(TB3CCR1 >= 1100) // Adjust the lower limit
                TB3CCR1 -= 100;                     // decreasing the duty cycle by 10%
            else
                TB3CCR1 = 1000; // Set duty cycle to 0 to stop the motor
        }
        else {
            if(TB3CCR1 <= 1900) // Adjust the upper limit
                TB3CCR1 += 100;                     // increasing the duty cycle by 10%
            else
                TB3CCR1 = 2000; // Set duty cycle to 0 to stop the motor
        }
    }
}

// Configure ADC Light
void ConfigureAdc_light(){
ADCCTL0 |= ADCSHT_8 | ADCON; // ADC ON,light sample period>30us
ADCCTL1 |= ADCSHP|ADCCONSEQ_2;//|ADCSSEL_2; // s/w trig, single ch/conv, MODOSC(5MHz)
ADCCTL2 &= ~ADCRES; // clear ADCRES in ADCCTL
ADCCTL2 |= ADCRES_2|ADCSR; // 12-bit conversion results
ADCMCTL0 |= ADCSREF_0 | ADCINCH_3; // ADC input ch A3 => light sense
ADCIE |=ADCIE0;
}

void TimerInit() //
{
    // Configure Timer_B3
    TB3CTL = TBSSEL_2 | MC__UP | TBCLR; // Remove TBIE flag
    TB3CCTL1 = OUTMOD_7;                // CCR1 reset/set
    TB3CCR0 = 20000;                    // 20kHz frequency
    TB3CCR1 = 1500;                     // starting duty cycle
}

void GPIOInit()
{
    //Pin used for the Servo
    P6OUT &= ~BIT0;     // Set P6.0 to 0
    P6DIR |= BIT0;      // Set P6.0 as output
    P6SEL0 |= BIT0;     // Select PWM
    P6SEL1 &= ~BIT0;    // Clear bit
}
