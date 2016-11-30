#Architecture 1: Project 3 - mamorales15

### This directory contains
* Code to play a simple Snake game on a Texas Intrsuments MSP430

### Functionality
* Uses the green LEDs on the board
* Uses the buttons S1-S4 on the EduKit BoosterPack v3
* Uses the speaker on the EduKit BoosterPack v3
* Uses the LCD display on the EduKit BoosterPack v3

### How to use
To compile:
~~~
$ make
~~~
Note: make is to be ran in arch1-project3-mamorales15 directory

To load onto MSP430:
~~~
$ make load
~~~
Note: make load is to be ran in arch1-project-3-mamorales15/SnakeGame directory

To delete binaries:
~~~
make clean
~~~
Note: make is to be ran in arch1-project3-mamorales15 directory

### How to play
1. Start the MSP430 by plugging it into a USB port
2. Control the snake using the four buttons on the EduKit BoosterPack v3.
   * S1 -> Up
   * S2 -> Down
   * S3 -> Left
   * S4 -> Right
3. Avoid hitting the wall! Hitting the wall will play an end of game sound, reset the score, and start the game over.
4. Try to eat the red food! Eating the food will increase your score. Any time a food is eaten, it will reappear in another randomly chosen spot on the board.
5. Be careful! The snake moves faster as your score increases.

### What to improve
Unfortunately, I ran out of time to complete the project.
There are still some bugs at the moment such as:* The game crashes in different ways. When the snake hits the food in a certain direction it can crash. Especially for the first collision. After a few wall collisions it can crash, but not always.
Some features I would have liked to implement:
* At the moment, the snake only has the head. Every time a food is eaten, a new moving layer should be created that the previous snake head/body part should link to. The pos of the snake body can take the posLast of the previous snake body/head.
* A song while you played would be nice.