/*
This section is the code that makes the DC Motor an output for our final project.
*/

#include <msp430.h>

void GPIOInit();
void TimerInit();

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer

    GPIOInit();
    TimerInit();

    PM5CTL0 &= ~LOCKLPM5;
    __bis_SR_register(GIE);

    while(1) // Add an infinite loop
    {
        // You can add any background tasks or power-saving code here
    }
}
void GPIOInit()
{
    //Pin used for the Servo
    P6OUT &= ~BIT0;     // Set P6.0 to 0
    P6DIR |= BIT0;      // Set P6.0 as output
    P6SEL0 |= BIT0;     // Select PWM
    P6SEL1 &= ~BIT0;    // Clear bit

    //Config button 4.1
    P4DIR &= ~BIT1;   // Setting Pin 4.1 to an input
    P4OUT |= BIT1;
    P4REN |= BIT1;    // using pull up resistor
    P4IE  |= BIT1;    // setting it to an interrupt
    P4IES |= BIT1;    // Configure the interrupt on the falling edge

    //Config button 2.3
    P2DIR &= ~BIT3;   // Setting Pin 2.3 to an input
    P2OUT |= BIT3;
    P2REN |= BIT3;    // using a pull up resistor
    P2IE  |= BIT3;    // Setting it to an interrupt
    P2IES |= BIT3;    // Configure the interrupt on the falling edge
}

void TimerInit() //
{
    // Configure Timer_B3
    TB3CTL = TBSSEL_2 | MC__UP | TBCLR; // Remove TBIE flag
    TB3CCTL1 = OUTMOD_7;                // CCR1 reset/set
    TB3CCR0 = 20000;                    // 20kHz frequency
    TB3CCR1 = 1500;                     // starting duty cycle
}



// Port 2 interrupt
#pragma vector=PORT2_VECTOR
__interrupt void Port_2(void)
{
    P2IFG &= ~BIT3;                         // Clearing P2.3 interrupt
    if(TB3CCR1 >= 1100) // Adjust the lower limit
        TB3CCR1 -= 100;                     // decreasing the duty cycle by 10%
    else
        TB3CCR1 = 1000;                  // Hard stop
}
// Port 4 interrupt
#pragma vector=PORT4_VECTOR
__interrupt void Port_4(void)
{
    P4IFG &= ~BIT1;                         // Clearing P4.1 interrupt
    if(TB3CCR1 <= 1900) // Adjust the upper limit
        TB3CCR1 += 100;                     // increasing the duty cycle by 10%
    else
        TB3CCR1 = 2000;                  // Hard stop
}
