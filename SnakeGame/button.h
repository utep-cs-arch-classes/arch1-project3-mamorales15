#ifndef button_included
#define button_included

#define BTN1 BIT0  // switch1 is p1.3
#define BTN2 BIT1
#define BTN3 BIT2
#define BTN4 BIT3
#define BUTTONS (BTN1 | BTN2 | BTN3 | BTN4)
void button_init();
void button_interrupt_handler();

extern char button_state_down, button_state_changed;

#endif
