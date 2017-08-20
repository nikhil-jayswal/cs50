//
// breakout.c
//
// Computer Science 50
// Problem Set 4
//

// standard libraries
#define _XOPEN_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Stanford Portable Library
#include "gevents.h"
#include "gobjects.h"
#include "gwindow.h"

// height and width of game's window in pixels
#define HEIGHT 600
#define WIDTH 400

// height and width of paddle
#define P_HEIGHT 10
#define P_WIDTH 60

// number of rows of bricks
#define ROWS 5

// number of columns of bricks
#define COLS 10

// radius of ball in pixels
#define RADIUS 10

// lives
#define LIVES 3

// prototypes
void initBricks(GWindow window);
GOval initBall(GWindow window);
GRect initPaddle(GWindow window);
GLabel initScoreboard(GWindow window);
void updateScoreboard(GWindow window, GLabel label, int points);
GObject detectCollision(GWindow window, GOval ball);

int main(void)
{
    // seed pseudorandom number generator
    srand48(time(NULL));

    // instantiate window
    GWindow window = newGWindow(WIDTH, HEIGHT);

    // instantiate bricks
    initBricks(window);

    // instantiate ball, centered in middle of window
    GOval ball = initBall(window);

    // instantiate paddle, centered at bottom of window
    GRect paddle = initPaddle(window);

    // instantiate scoreboard, centered in middle of window, just above ball
    GLabel label = initScoreboard(window);

    // number of bricks initially
    int bricks = COLS * ROWS;

    // number of lives initially
    int lives = LIVES;

    // number of points initially
    int points = 0;
    
    // initial velocity (X and Y components) of ball
    double Xvelocity = 2 * drand48(); 
    double Yvelocity = 4.5;
        
    //wait for click to start the game
    waitForClick();
    
    // keep playing until game over
    while (lives > 0 && bricks > 0)
    {
            // move paddle corresponding to mouse cursor movement
            {
                // check for mouse event
                GEvent event = getNextEvent(MOUSE_EVENT);
                
                // if we heard one
                if (event != NULL)
                {
                    // if the event was movement
                    if (getEventType(event) == MOUSE_MOVED)
                    {
                        // ensure paddle follows cursor within window boundaries
                        double x = getX(event) - getWidth(paddle) / 2;
                        
                        if (x <= 0)
                            setLocation(paddle, 0, HEIGHT - 50 - P_HEIGHT);
                        
                        else if (x >= WIDTH - getWidth(paddle))
                            setLocation(paddle, WIDTH - getWidth(paddle), HEIGHT - 50 - P_HEIGHT);
                        
                        else
                            setLocation(paddle, x, HEIGHT - 50 - P_HEIGHT);
                    }
                }
            }
            
            // move and bounce ball off edges, bricks and paddle and take appropriate action
            {
                move(ball, Xvelocity, Yvelocity);
                
                // bounce off right edge of window
                if (getX(ball) + getWidth(ball) >= getWidth(window))
                {
                    Xvelocity = -Xvelocity;
                    move(ball, Xvelocity, Yvelocity);
                }

                // bounce off left edge of window
                else if (getX(ball) <= 0)
                {
                    Xvelocity = -Xvelocity;
                    move(ball, Xvelocity, Yvelocity); 
                }
                
                // bounce off top edge
                else if (getY(ball) <= 0)
                {
                    Yvelocity = -Yvelocity;
                    move(ball, Xvelocity, Yvelocity); 
                }
                
                // when ball collides with the bottom edge 
                // decrease number of available lives
                // reset ball and paddle position
                // and pause game until user clicks
                else if (getY(ball) + getHeight(ball) >= getHeight(window))
                {
                    // decrease number of available lives
                    lives = lives - 1;
                    
                    // reset only if lives left
                    if(lives > 0)  
                    {   
                        // wait before reset
                        pause(2);
                        
                        // reset ball and paddle location, ball velocity and wait for click
                        setLocation(ball, WIDTH / 2 - RADIUS, HEIGHT / 2 - RADIUS);
                        setLocation(paddle, WIDTH / 2 - P_WIDTH / 2, HEIGHT - 50 - P_HEIGHT);
                        
                        srand48(time(NULL));
                        Xvelocity = 2 * drand48();
                        Yvelocity = 4.5;
                        
                        waitForClick();
                    }
                }
                
                // detect collision of ball with any object
                GObject object = detectCollision(window, ball);
                               
                if(object != NULL)
                {   
                    // bounce off paddle
                    if (object == paddle)
                    {
                        // X and Y coordinates of ball
                        double ball_X = getX(ball);
                        double ball_Y = getY(ball);
                        
                        // X and Y coordinates of paddle
                        double paddle_X = getX(paddle);
                        double paddle_Y = getY(paddle);                        
                        
                        // bounce off paddle sides
                        if(ball_Y + 2 * RADIUS > paddle_Y)
                        {
                            Xvelocity = -Xvelocity;
                            move(ball, Xvelocity, Yvelocity);    
                        }
                        
                        // if ball near paddle corners, bounce at an angle
                        if (ball_X <= paddle_X + 2 && ball_X <= paddle_X + P_WIDTH - 2)
                        {
                            Yvelocity = -Yvelocity;
                            Xvelocity = Xvelocity + 2;
                            move(ball, Xvelocity, Yvelocity);  
                        }
                        
                        else    
                        {
                            Yvelocity = -Yvelocity;
                            move(ball, Xvelocity, Yvelocity);
                        } 
                    }
                    
                    // when ball collides with bricks...
                    else if(strcmp(getType(object), "GRect") == 0)
                    {
                        // bounce off brick
                        Yvelocity = -Yvelocity;
                        move(ball, Xvelocity, Yvelocity); 
                        
                        // remove the brick from window
                        removeGWindow(window, object);
                        
                        // update number of bricks
                        bricks = bricks - 1; 
                        
                        // update score in window
                        points = points + 1;
                        updateScoreboard(window, label, points);
                    }
                }
                
                // linger before moving again
                pause(15);
            }  
    }

    // wait for click before exiting
    // waitForClick();      // don't need this here

    // when game over
    setFont(label,"Sans-20");
    setLabel(label,"GAME OVER! Click anywhere to Exit.");
    
    // center label in window
    double x = (getWidth(window) - getWidth(label)) / 2;
    double y = (getHeight(window) - getHeight(label)) / 2;
    setLocation(label, x, y);
    
    // wait for click before exiting
    waitForClick();
    closeGWindow(window);
    return 0;
}

/**
 * Initializes window with a grid of bricks.
 */
void initBricks(GWindow window)
{
    // horizontal and  vertical gap between bricks and height and width of bricks
    int Xgap = 5;
    int Ygap = 5;
    int brick_height = 10;
    int brick_width = 35;
    
    // X coordinate of upper left corner of first brick
    int Xstart = (WIDTH - COLS * brick_width - (COLS - 1) * Xgap) / 2;
    
    // print bricks inside window 
    for (int i = 0; i < COLS; ++i)
    {
        // Y coordinate of upper left corner of first brick of each column 
        int Ystart = 50;
        
        for (int j = 0; j < ROWS; ++j)
        {
            GRect brick = newGRect(Xstart, Ystart, brick_width, brick_height);
            
            // assign color to brick
            switch (j+1)
            {
                case 1:
                    setColor(brick, "RED");
                    break;
                
                case 2:
                    setColor(brick, "ORANGE");
                    break;
                
                case 3:
                    setColor(brick, "YELLOW");
                    break;
                
                case 4:
                    setColor(brick, "GREEN");
                    break;
                
                case 5:
                    setColor(brick, "BLUE");
                    break;
                
                default:
                    setColor(brick, "BLACK");
                    break;
            }
            
            // fill brick with color
            setFilled(brick, true);
            
            // add brick to window
            add(window, brick);
            
            Ystart = Ystart + brick_height + Ygap;
        }
        
        Xstart = Xstart + brick_width + Xgap;
    }
}

/**
 * Instantiates ball in center of window.  Returns ball.
 */
GOval initBall(GWindow window)
{
    // instantiate a ball in window's center
    GOval ball = newGOval(WIDTH / 2 - RADIUS, HEIGHT / 2 - RADIUS, 2 * RADIUS, 2 * RADIUS);
    setColor(ball, "BLACK");
    setFilled(ball, true); 
    add(window, ball);      
    
    return ball;
}

/**
 * Instantiates paddle in bottom-middle of window.
 */
GRect initPaddle(GWindow window)
{
    // X and Y coordinates of upper left corner of paddle
    int Xcorner = WIDTH / 2 - P_WIDTH / 2;
    
    int Ycorner = HEIGHT - 50 - P_HEIGHT;  
   
    GRect paddle = newGRect (Xcorner, Ycorner, P_WIDTH, P_HEIGHT);
    setColor(paddle, "BLACK");
    setFilled(paddle, true);
    add(window, paddle);        
    
    return paddle;
}

/**
 * Instantiates, configures, and returns label for scoreboard.
 */
GLabel initScoreboard(GWindow window)
{
    // create a label for score
    GLabel label;
    label = newGLabel("0");
    setColor(label, "GRAY");
    setFont(label,"Sans-48");
    
    // add label to window
    add(window, label);
    
    // center label in window
    double x = (getWidth(window) - getWidth(label)) / 2;
    double y = (getHeight(window) - getHeight(label)) / 2;
    setLocation(label, x, y);
     
    return label;
}

/**
 * Updates scoreboard's label, keeping it centered in window.
 */
void updateScoreboard(GWindow window, GLabel label, int points)
{
    // update label
    char s[12];
    sprintf(s, "%i", points);
    setLabel(label, s);

    // center label in window
    double x = (getWidth(window) - getWidth(label)) / 2;
    double y = (getHeight(window) - getHeight(label)) / 2;
    setLocation(label, x, y);
}

/**
 * Detects whether ball has collided with some object in window
 * by checking the four corners of its bounding box (which are
 * outside the ball's GOval, and so the ball can't collide with
 * itself).  Returns object if so, else NULL.
 */
GObject detectCollision(GWindow window, GOval ball)
{
    // ball's location
    double x = getX(ball);
    double y = getY(ball);

    // for checking for collisions
    GObject object;

    // check for collision at ball's top-left corner
    object = getGObjectAt(window, x, y);
    if (object != NULL)
    {
        return object;
    }

    // check for collision at ball's top-right corner
    object = getGObjectAt(window, x + 2 * RADIUS, y);
    if (object != NULL)
    {
        return object;
    }

    // check for collision at ball's bottom-left corner
    object = getGObjectAt(window, x, y + 2 * RADIUS);
    if (object != NULL)
    {
        return object;
    }

    // check for collision at ball's bottom-right corner
    object = getGObjectAt(window, x + 2 * RADIUS, y + 2 * RADIUS);
    if (object != NULL)
    {
        return object;
    }

    // no collision
    return NULL;
}
