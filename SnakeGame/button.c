#include <msp430.h>
#include "button.h"

char button1_state_down, button2_state_down, button3_state_down, button4_state_down;
char button_state_changed;

static char button_update_interrupt_sense() {
  char p2val = P2IN;
  /* update button interrupt sensitivity */
  P2IES |= (p2val & BUTTONS); // if button is up, sense is down
  P2IES &= (p2val | ~BUTTONS); // if button is down, sense is up
  return p2val;
}

void button_init() {
  P2REN |= BUTTONS; // enables resistors for buttons
  P2IE = BUTTONS; // enables interrupts from buttons
  P2OUT |= BUTTONS; // pull-ups for buttons
  P2DIR &= ~BUTTONS; // set buttons' bits for input
  button_update_interrupt_sense();
  button_interrupt_handler(); // to initially read the buttons
}

void button_interrupt_handler() {
  char p2val = button_update_interrupt_sense();
  button1_state_down = (p2val & BTN1) ? 0 : 1; // 0 when BTN1 is up
  button2_state_down = (p2val & BTN2) ? 0 : 1; // 0 when BTN2 is up
  button3_state_down = (p2val & BTN3) ? 0 : 1; // 0 when BTN3 is up
  button4_state_down = (p2val & BTN4) ? 0 : 1; // 0 when BTN4 is up
  button_state_changed = 1;
}

int getButtonPressed() {
  int btnPressed = -1;
  if(button1_state_down == 1) {
    btnPressed = 1;
  }
  else if(button2_state_down == 1) {
    btnPressed = 2;
  }
  else if(button3_state_down == 1) {
    btnPressed = 3;
  }
  else if(button4_state_down == 1) {
    btnPressed = 4;
  }
  else {
    btnPressed = -1;
  }
  return btnPressed;
}









