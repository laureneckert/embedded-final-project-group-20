/*
This section is the code to make the onboard light sensor on the MSP430fr2355 an input for the final project
*/

#include <msp430.h>

volatile long light, lightRaw;

void ConfigureAdc_light();

void main(void)
{
WDTCTL = WDTPW + WDTHOLD; // Stop watchdog timer
__delay_cycles(5); // Wait for ADC Ref to settle
__delay_cycles(400);
ConfigureAdc_light();
while(1){
ADCCTL0 |=ADCENC | ADCSC; //Enable and start conversion

while ((ADCIFG &ADCIFG0)==0);
light = ADCMEM0; // read the converted data into a variable
}
}

// Configure ADC Temperature
void ConfigureAdc_light(){
ADCCTL0 |= ADCSHT_8 | ADCON; // ADC ON,temperature sample period>30us
ADCCTL1 |= ADCSHP|ADCCONSEQ_2;//|ADCSSEL_2; // s/w trig, single ch/conv, MODOSC(5MHz)
ADCCTL2 &= ~ADCRES; // clear ADCRES in ADCCTL
ADCCTL2 |= ADCRES_2|ADCSR; // 12-bit conversion results
ADCMCTL0 |= ADCSREF_0 | ADCINCH_3; // ADC input ch A3 => temp sense
ADCIE |=ADCIE0;
}
