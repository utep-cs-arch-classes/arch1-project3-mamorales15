/** \file snake.c
 *  \brief This is a simple shape motion demo.
 *  This demo creates two layers containing shapes.
 *  One layer contains a rectangle and the other a circle.
 *  While the CPU is running the green LED is on, and
 *  when the screen does not need to be redrawn the CPU
 *  is turned off along with the green LED.
 */  
#include <msp430.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libTimer.h>
#include <lcdutils.h>
#include <lcddraw.h>
#include <shape.h>
#include <abCircle.h>
#include "button.h"
#include "tone.h"
#define GREEN_LED BIT6
#define RED_LED BIT0

char scoreTen = 0;
char scoreOne = 0;
int score = 0;
char scoreMess[10] = "Score: ";
int buzzerOn = 0;

AbRect rect5 = {abRectGetBounds, abRectCheck, {5,5}}; /**< 5x5 rectangle */

AbRectOutline fieldOutline = {	/* playing field */
  abRectOutlineGetBounds, abRectOutlineCheck,   
  {screenWidth/2 - 10, screenHeight/2 - 10}
};

Layer fieldLayer2 = {		/* playing field as a layer */
  (AbShape *) &fieldOutline,
  {screenWidth/2, screenHeight/2},/**< center */
  {0,0}, {0,0},				    /* last & next pos */
  COLOR_BLACK,
  0
};

Layer foodLayer1 = {		/**< Layer with a red square */
  (AbShape *)&rect5,
  {screenWidth/2 + 30, screenHeight/2},
  {0,0}, {0,0},				    /* last & next pos */
  COLOR_RED,
  &fieldLayer2,
};

Layer snakeLayer0 = {		/**< Layer with a red square */
  (AbShape *)&rect5,
  {screenWidth/2, screenHeight/2}, /**< center */
  {0,0}, {0,0},				    /* last & next pos */
  COLOR_GREEN,
  &foodLayer1,
};


/** Moving Layer
 *  Linked list of layer references
 *  Velocity represents one iteration of change (direction & magnitude)
 */
typedef struct MovLayer_s {
  Layer *layer;
  Vec2 velocity;
  struct MovLayer_s *next;
} MovLayer;

/* initial value of {0,0} will be overwritten */
// MovLayer ml3 = { &layer3, {1,1}, 0 }; /**< not all layers move */
// MovLayer ml1 = { &layer1, {1,2}, &ml3 }; 
MovLayer food = { &foodLayer1, {0,0}, 0 };
MovLayer snakeHead = { &snakeLayer0, {1,0}, &food };

int myRand(int min, int max) {
  // Not uniform randonmess but suits my purpose
  return (rand() & (max + 1 - min)) + min; // Credit: StackOverflow
}

void generateFood() {
  int randCol = myRand(0, screenWidth);
  int randRow = myRand(0, screenHeight);
  /*
  Layer foodLayer1 = {
    (AbShape *)&rect5,
    {randCol, randRow},
    {0,0}, {0,0},
    // last & new pos
    COLOR_RED,
    &fieldLayer2,
  };
  */
  Vec2 newPos = {
    {randCol, randRow}
  };
  //scoreOne = newPos.axes[0];
  foodLayer1.posNext = newPos;
  //scoreTen = foodLayer1.posNext.axes[0];
}

void createNewSnake() {
  Layer snakeLayer0 = {		/**< Layer with a red square */
  (AbShape *)&rect5,
  {screenWidth/2, screenHeight/2}, /**< center */
  {0,0}, {0,0},				    /* last & next pos */
  COLOR_GREEN,
  &foodLayer1,
};
  //snakeHead = { &snakeLayer0, {1,0}, 0 };
  snakeHead.layer = &snakeLayer0;
}
  
void newGame() {
  generateFood();
  createNewSnake();
}

void moveLayer(Vec2 *result, const Vec2 *v, int col, int row) {
  result->axes[0] = col;
  result->axes[1] = row;
}

void resetScore() {
  scoreTen = 0;
  scoreOne = 0;
  score = 0;
}

void incrementScore() {
  if(scoreOne == 9) { // Carry over to tens place
    scoreOne = 0;
    if(scoreTen == 9) { // Limit on score is 99. Reset it to 0 after that.
      scoreTen = 0;
    } else {
      scoreTen = scoreTen + 1;
    }
  } else {
    scoreOne = scoreOne + 1; // Increment ones
  }
  score = score + 1;
}

movLayerDraw(MovLayer *movLayers, Layer *layers)
{
  int row, col;
  MovLayer *movLayer;

  and_sr(~8);			/**< disable interrupts (GIE off) */
  for (movLayer = movLayers; movLayer; movLayer = movLayer->next) { /* for each moving layer */
    Layer *l = movLayer->layer;
    l->posLast = l->pos;
    l->pos = l->posNext;
  }
  or_sr(8);			/**< disable interrupts (GIE on) */


  for (movLayer = movLayers; movLayer; movLayer = movLayer->next) { /* for each moving layer */
    Region bounds;
    layerGetBounds(movLayer->layer, &bounds);
    lcd_setArea(bounds.topLeft.axes[0], bounds.topLeft.axes[1], 
		bounds.botRight.axes[0], bounds.botRight.axes[1]);
    for (row = bounds.topLeft.axes[1]; row <= bounds.botRight.axes[1]; row++) {
      for (col = bounds.topLeft.axes[0]; col <= bounds.botRight.axes[0]; col++) {
	Vec2 pixelPos = {col, row};
	u_int color = bgColor;
	Layer *probeLayer;
	for (probeLayer = layers; probeLayer; 
	     probeLayer = probeLayer->next) { /* probe all layers, in order */
	  if (abShapeCheck(probeLayer->abShape, &probeLayer->pos, &pixelPos)) {
	    color = probeLayer->color;
	    break; 
	  } /* if probe check */
	} // for checking all layers at col, row
	lcd_writeColor(color); 
      } // for col
    } // for row
  } // for moving layer being updated
}	  

//Region fence = {{10,30}, {SHORT_EDGE_PIXELS-10, LONG_EDGE_PIXELS-10}}; /**< Create a fence region */

/** Advances a moving shape within a fence
 *  
 *  \param ml The moving shape to be advanced
 *  \param fence The region which will serve as a boundary for ml
 */
void mlAdvance(MovLayer *ml, Region *fence)
{
  // Display score
  scoreMess[7] = '0' + scoreTen;
  scoreMess[8] = '0' + scoreOne;
  drawString5x7(20,20, scoreMess, COLOR_BLACK, bgColor);
  
  
  Vec2 newPos;
  u_char axis;
  Region shapeBoundary;
  
  for (; ml; ml = ml->next) {
    if(score > 0){
      Vec2 newVelocity = {
	{score,score}
      };
      ml->velocity = newVelocity;
    }
    vec2Add(&newPos, &ml->layer->posNext, &ml->velocity);
    abShapeGetBounds(ml->layer->abShape, &newPos, &shapeBoundary);
    for (axis = 0; axis < 2; axis ++) {
      if ((shapeBoundary.topLeft.axes[axis] < fence->topLeft.axes[axis]) ||
	  (shapeBoundary.botRight.axes[axis] > fence->botRight.axes[axis]) ) {
        // drawString5x7(30, 75, "GAME OVER!!!", COLOR_RED, bgColor);

	moveLayer(&newPos, &ml->layer->posNext, screenWidth/2, screenHeight/2);
	resetScore();
	//free(&foodLayer1);
	//free(&snakeLayer0);
	//newGame();
	generateFood();
	
	// Play end game tone
	tone_set_period(500);
	buzzerOn = 1;
	
      }	/**< if outside of fence */
    } /**< for axis */
    ml->layer->posNext = newPos;
  } /**< for ml */
}

u_int bgColor = COLOR_WHITE;     /**< The background color */
int redrawScreen = 1;           /**< Boolean for whether screen needs to be redrawn */

Region fieldFence;		/**< fence around playing field  */

/** Initializes everything, enables interrupts and green LED, 
 *  and handles the rendering for the screen
 */
void main()
{
  P1DIR |= GREEN_LED;		/**< Green led on when CPU on */		
  P1OUT |= GREEN_LED;

  configureClocks();
  lcd_init();
  tone_init();
  shapeInit();
  button_init();
  
  layerInit(&snakeLayer0);
  layerDraw(&snakeLayer0);

  layerGetBounds(&fieldLayer2, &fieldFence);


  enableWDTInterrupts();      /**< enable periodic interrupt */
  or_sr(0x8);	              /**< GIE (enable interrupts) */


  for(;;) {
    while (!redrawScreen) { /**< Pause CPU if screen doesn't need updating */
      P1OUT &= ~GREEN_LED;    /**< Green led off witHo CPU */
      or_sr(0x10);	      /**< CPU OFF */
    }

    // Turn off buzzer after 5 cycles
    if(buzzerOn >= 1) {
      buzzerOn = buzzerOn + 1;
      if(buzzerOn == 5) {
	tone_set_period(0);
	buzzerOn = 0;
      }
    }
    
    P1OUT |= GREEN_LED;       /**< Green led on when CPU on */
    redrawScreen = 0;
    movLayerDraw(&snakeHead, &snakeLayer0);
  }
}

void moveSnakePieces(MovLayer *movLayers) {
  MovLayer *movLayer;
  
  for(movLayer = movLayers; movLayer; movLayer = movLayer->next) {
    if(getButtonPressed() == 1) { // Move up
      movLayer->velocity.axes[0] = 0;
      movLayer->velocity.axes[1] = -1;
    }
    if(getButtonPressed() == 2) { // Move down
      movLayer->velocity.axes[0] = 0;
      movLayer->velocity.axes[1] = 1;
    }
    if(getButtonPressed() == 3) { // Move left
      movLayer->velocity.axes[0] = -1;
      movLayer->velocity.axes[1] = 0;
    }
    if(getButtonPressed() == 4) { // Move right
      movLayer->velocity.axes[0] = 1;
      movLayer->velocity.axes[1] = 0;
    }
  }
}

/** Watchdog timer interrupt handler. 15 interrupts/sec */
void wdt_c_handler()
{
  static short count = 0;
  P1OUT |= GREEN_LED;		      /**< Green LED on when cpu on */
  count ++;
  if (count == 15) {
    moveSnakePieces(&snakeHead);
    mlAdvance(&snakeHead, &fieldFence);
    
    redrawScreen = 1;
    count = 0;
  }
  P1OUT &= ~GREEN_LED;		    /**< Green LED off when cpu off */
}
