#include <msp430.h> 

unsigned int memo_capt;
unsigned int capt;
unsigned int diff_capt;
unsigned short Flag;
unsigned int distance = 10;
int IR = 0;

int ADCread()
{

    init_adc(0x07);
    __delay_cycles(15);
    IR = read_adc(0x07);
    ;
    __delay_cycles(15);

    return IR;
}

void trigger()

{

    P1OUT |= BIT4;
    __delay_cycles(10);
    P1OUT &= ~(BIT4);

}



/**
 * main.c
 */

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
    BCSCTL1 = CALBC1_1MHZ;
    DCOCTL = CALDCO_1MHZ;

    P1DIR |= BIT4;          // trigger pin as output
    P1OUT &= ~BIT4;

    P1IE |= BIT2;           // set ultrasonic sensor input as interrupt
    P1IFG &= ~BIT2;
    P1SEL |= BIT2;
    P1DIR |= BIT4;



    P2DIR |= (BIT1 | BIT2 | BIT4 | BIT5);
    P2SEL |= (BIT2 | BIT4);
    P2SEL &= ~(BIT1 | BIT5);
    P2SEL2 &= ~(BIT1 | BIT2 | BIT4 | BIT5);

    //set timer interrupt

    TA0CTL = (TASSEL_2 | MC_2);
    TA0CCTL1 = CM_3 + CAP + CCIS_0 + SCS + CCIE;
    TA0CCTL1 &= ~ CCIFG;

    Flag = 0;
    _BIS_SR(GIE);

    //set pwm
    TA1CTL = TASSEL_2 | MC_1 | ID_3;
    TA1CCTL1 |= OUTMOD_7;
    TA1CCTL2 |= OUTMOD_7;

    TA1CCR0 = 1000;
    TA1CCR1 = 100;
    TA1CCR2 = 100;



    P2OUT &= ~BIT1;
    P2OUT |= (BIT5);


    while (1)
    {
        ADCread(0x07);
        trigger(0x04);
        __delay_cycles(30000);
        if (distance < 15)
        {
            if (IR < 700){

            P2OUT |= (BIT1);
            TA1CCR1 = 100;
            TA1CCR2 = 100;
            }
            else {
                P2OUT &= ~(BIT5);
                           TA1CCR1 = 100;
                           TA1CCR2 = 100;
                           __delay_cycles(35000);
            }
        }
        else
        {
          if  (IR > 700)
              {
            P2OUT &= ~(BIT5);
            TA1CCR1 = 100;
            TA1CCR2 = 100;
            __delay_cycles(35000);
              }
          else
        {
            P2OUT &= ~BIT1;
            P2OUT |= (BIT5);
            TA1CCR1 = 100;
            TA1CCR2 = 100;
        }
         }

    }
    return 0;
}

#pragma vector=TIMER0_A1_VECTOR
__interrupt void Timer0_A1(void)
{
    switch (TA0IV)
    {
    case (TA0IV_TACCR1):

    {
        if (Flag == 0)
        {
            memo_capt = TA0CCR1;
            Flag = 1;
        }
        else
        {
            capt = TA0CCR1;
            diff_capt = capt - memo_capt;
            Flag = 0;
            TA0CCR1 = 0; //
            distance = (diff_capt) / 58;

        }
    }
    }
}

