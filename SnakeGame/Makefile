# makfile configuration
CPU             	= msp430g2553
CFLAGS          	= -mmcu=${CPU} -Os -I../h -I../h

#switch the compiler (for the internal make rules)
CC              = msp430-elf-gcc
AS              = msp430-elf-gcc -mmcu=${CPU} -c

all:snake.elf

#additional rules for files
snake.elf: ${COMMON_OBJECTS} p2_interrupt_handler.o tone.o snake.o button.o wdt_handler.o
	${CC} -mmcu=${CPU}   -o $@ $^ -L../lib -lTimer -lLcd -lShape -lCircle -lp2sw
load: snake.elf
	mspdebug rf2500 "prog $^"

clean:
	rm -f *.o *.elf

tone.c: tone.h
snake.c: button.h
button.c: button.h
p2_interrupt_handler.c: button.h
