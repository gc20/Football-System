//***********************************
// constant definitions
//***********************************

// field constants
#define FTop 0          // y coordinate for the top of the  field that is the wall closest to the top of your screen.
#define FBottom 400     // y coordinate for the bottom of  the field that is the wall closest to the bottom of your screen. 
#define FLeft 20        // x coordinate for the Left wall of  the field that is the wall closest to the Left side of your screen.
#define FRight 620      // x coordinate for the right wall  of the field that is the wall closest to the right side of your screen. 
#define GUpper 170      // y coordinate of the opening of the goal on the upper side of the screen
#define GLower 230      // y coordinate of the opening of the goal on the lower side of the screen

// player and ball constants
#define playerRadius 7  // Player is 7 pixels in radius
#define ballRadius 5    // Ball is 5 pixels in radius
#define kickRadius 15   // If distance between centers of ball and player are <= 15 pixels, player is allowed to kick the ball
#define NUMBER_OF_PLAYERS 5 // Number of players in each team
#define MAX_DISTANCE 10000 // Maximum distance between any 2 objects

// foul constants
#define FOUL_BOX_1_TOP_RIGHT_X 0
#define FOUL_BOX_1_TOP_RIGHT_Y 160
#define FOUL_BOX_1_BOTTOM_LEFT_X 50
#define FOUL_BOX_1_BOTTOM_LEFT_Y 240
#define FOUL_BOX_2_TOP_RIGHT_X 580
#define FOUL_BOX_2_TOP_RIGHT_Y 160
#define FOUL_BOX_2_BOTTOM_LEFT_X 600
#define FOUL_BOX_2_BOTTOM_LEFT_Y 240

// ball initial values
#define BALL_INITIAL_POSITION_X 0
#define BALL_INITIAL_POSITION_Y 0
#define BALL_INITIAL_SPEED 0    // binary : 0000
#define BALL_INITIAL_DIRECTION 0

// misc
#define PI 3.14159265
#define qSize 25
#define timeperiod 0.04 // Time period for 25Hz
#define DEBUG_MODE 0
