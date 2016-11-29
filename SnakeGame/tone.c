#include <msp430.h>
#include "tone.h"

void tone_set_period(short cycles) {
  CCR0 = cycles;
  CCR1 = cycles >> 1;
}

void tone_init() {
  timerAUpmode();  // used to drive speaker
  P2SEL2 &= ~(BIT6 | BIT7);
  P2SEL &= ~BIT7;
  P2SEL |= BIT6;
  P2DIR = BIT6;  // enable output to speaker
}

