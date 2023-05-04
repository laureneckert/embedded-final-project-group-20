/*
This code controls a DC motor using a microcontroller for a final project.
*/

#include <msp430.h>

void GPIOInit(); // Function to initialize GPIO pins
void TimerInit(); // Function to initialize the timer

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // Stop watchdog timer

    GPIOInit(); // Initialize GPIO pins
    TimerInit(); // Initialize timer

    PM5CTL0 &= ~LOCKLPM5; // Unlock LPM5 to enable GPIO pins
    __bis_SR_register(GIE); // Enable global interrupts

    while(1) // Infinite loop
    {
        // Add background tasks or power-saving code here
    }
}

void GPIOInit()
{
    // Configure pin P6.0 for the motor
    P6OUT &= ~BIT0;     // Set P6.0 to 0
    P6DIR |= BIT0;      // Set P6.0 as output
    P6SEL0 |= BIT0;     // Select PWM
    P6SEL1 &= ~BIT0;    // Clear bit

    // Configure button on P4.1
    P4DIR &= ~BIT1;   // Set P4.1 as input
    P4OUT |= BIT1;    // Enable internal pull-up resistor
    P4REN |= BIT1;    // Enable pull-up resistor
    P4IE  |= BIT1;    // Enable interrupt
    P4IES |= BIT1;    // Set interrupt on falling edge

    // Configure button on P2.3
    P2DIR &= ~BIT3;   // Set P2.3 as input
    P2OUT |= BIT3;    // Enable internal pull-up resistor
    P2REN |= BIT3;    // Enable pull-up resistor
    P2IE  |= BIT3;    // Enable interrupt
    P2IES |= BIT3;    // Set interrupt on falling edge
}

void TimerInit()
{
    // Configure Timer_B3
    TB3CTL = TBSSEL_2 | MC__UP | TBCLR; // Set SMCLK, up mode, clear timer
    TB3CCTL1 = OUTMOD_7;                // Set CCR1 to reset/set
    TB3CCR0 = 20000;                    // Set frequency to 20kHz
    TB3CCR1 = 1500;                     // Set initial duty cycle
}

// Interrupt handler for P2.3 button
#pragma vector=PORT2_VECTOR
__interrupt void Port_2(void)
{
    P2IFG &= ~BIT3;                     // Clear P2.3 interrupt flag
    if(TB3CCR1 >= 1100)                 // Check for lower limit
        TB3CCR1 -= 100;                 // Decrease duty cycle by 10%
    else
        TB3CCR1 = 1000;                 // Set hard stop at lower limit
}

// Interrupt handler for P4.1 button
#pragma vector=PORT4_VECTOR
__interrupt void Port_4(void)
{
    P4IFG &= ~BIT1;                     // Clear P4.1 interrupt flag
    if(TB3CCR1 <= 1900)                 // Check for upper limit
        TB3CCR1 += 100;                 // Increase duty cycle by 10%
    else
        TB3CCR1 = 2000;                 // Set hard stop at upper limit
}
