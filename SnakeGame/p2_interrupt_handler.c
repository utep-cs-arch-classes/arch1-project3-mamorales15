#include <msp430.h>
#include "button.h"

/* Buttons on P2 (S1-S4) */
__interrupt(PORT2_VECTOR) Port_2(){
  if(P2IFG & BUTTONS) {  // did a button cause this interrupt?
    P2IFG &= ~BUTTONS; // clear pending btn interrupts
    button_interrupt_handler(); // single handler for all buttons
  }
}
