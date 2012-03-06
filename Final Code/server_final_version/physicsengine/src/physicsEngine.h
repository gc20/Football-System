// include files
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <xparameters.h>
#include <xuartlite.h>
#include <xgpio.h>
#include "xmbox.h"
#include <time.h>

//definitions
#define qSize 25
#define MSGSIZE  16
#define PCK_SIZE 16
#define assumedPeriod 0.2
#define granularity 0.2

typedef struct position {
    double x, y;
} position;

typedef struct speed {
    double x, y;
} speed;

typedef struct kick {
	speed speed;
	#warning resolve speed magnitude and speed direction to speed.x and speed.y
	u8 kickBit;
} kick;

typedef struct player {
    position position;
    speed speed;
	u8 direction; // Just stored value provided by controller
	kick kick;
} player;

typedef struct ball {
    position position;
    speed speed;
	u8 direction;
	double theta; // In degrees
} ball;

typedef struct mailboxSend {
	int player0x, player1x, player2x, player3x, player4x, player5x, player6x, player7x, player8x, player9x;
	int player0y, player1y, player2y, player3y, player4y, player5y, player6y, player7y, player8y, player9y;
	int ballx, bally;
	int goalA;
	int goalB;
	int foulA;
	int foulB;
	int time;
} mailboxSend;

typedef enum gameState {
	NORMAL,
	END_GAME,
	DO_NOTHING,
	GOAL,
	FOULALeft,
	FOULARight,
	FOULBLeft,
	FOULBRight
} gameStates;


//Global variables are declared here

// Main
ball nike;
player playerAll[10];
position playerInitialCoordinates[10];
position playerLeftRandomPos[10];
position playerRightRandomPos[10];
gameStates gameState;
XGpio GpioInputButton;
XGpio GpioInputSwitches;
XStatus status;
XMbox mbox;
XMbox_Config *ConfigPtr;
mailboxSend mailboxSendData;
double tempSwap;
u8 playerToKick = 20;
u8 gameInPlay; // Check if game is to run

// Interrupt related variables
volatile u32 circularDataBuffer[qSize] = {0}, circularDataBufferDTE[qSize] = {0};
volatile u8 qRead = 0, qReadDTE = 0;
volatile u8 qWrite = 0, qWriteDTE = 0;
XUartLite uart_inst;
XUartLite uart_inst2;

// Scoreboard variables
u8 goalA;
u8 goalB;
u8 foulA;
u8 foulB;
u8 goalUpdateBit;
u8 foulUpdateBit; //-> 0-> no foul. 1-> A foul. 2-> B foul.
u8 gameBit = 0; // 0-> Do nothing. 1-> Run game during normal time. 2-> Normal time has ended. 3-> Start extra time. 4-> Extra time has ended.
u8 winnerBit; // 0-> Default(Game Not Over) 1-> team A wins.  2-> teamB wins. 3-> draw after normal time. 4-> draw after extra time.

// Timer variables
long int sendTime = 0; // Number to be displayed/sent
long int prevTime = 0; // System time in the last clock cycle
int countdownLimit = 0; // Time to which the clock should count
long int goalTime = 100; // Count two seconds
time_t t1;

// Game initialization
void settingUpGame();

//check initial player positions to ensure no overlapping. If overlapped, randomly generate another address
void checkInitialPlayerPos ();

// Initialize hardware interrupts
void initializeInterrupts ();

//The Receive Interrupt Handler for DTE
static void Recv_HandlerDTE (void* CallBackRef, unsigned int EventData);

//The Receive Interrupt Handler for DCE
static void Recv_HandlerDCE (void* CallBackRef, unsigned int EventData);

// Receive initial coordinates of players
void receiveInitialData ();

// Read and decode all packets stored in queue buffer
void readQueueData ();

// Whenever a packet is received, send it to this function, which will decode data appropriately
void decodeReceivedPacket (u32 RecvPacket);

// Send Interrupt Handler (Unused)
static void Send_Handler (void* CallBackRef, unsigned int EventData);

// Send 32-bit packet
void sendPacket(u32 playerPacket);

// Send control packet
void sendControlPacket ();

// Send ball info packet
void sendBallInfoPacket();

// Send player info packets
void sendPlayerInfoPackets ();

// Setup mailboxes
void initializeMailbox();

// Send information via mailbox
void sendViaMailbox();

// Initialize push buttons
void initializePushButtons ();

// Read GPIO input
int GpioInputInitialize(u32 DeviceId, XGpio *GpioInput);

// Check if push button has been pressed to indicate foul
void checkPushButtonFouls ();

// Update timer count
void updateTimer ();

// check for foul and goal
void checkFoulGoalConditions();

// Update speeds of ball and players
void calculateNewSpeeds();

// Check if player's kick is directionally valid
u8 checkBallInFront (u8 i);

// Check if player is allowed to kick
void checkPlayerToKick ();

// Run the game
void gamePlay ();

// Function which handles the core execution of the game
void updateBallPlayerPos();

// Compute distance between two positions
double computeDistance(double x1, double y1, double x2, double y2);

// Check if provided speed is legal
void checkSpeedLegality ();

// Collision Case 1: Check whether ball and player have collided
void checkBallColPlayer();

// Collision Case 2: check whether player and player have collided
void checkPlayerColPlayer();

// When ball collides with wall
void checkBallColWall();

// When player collides with wall
void checkPlayerColWall();

// returns 1 is there is collision, 0 otherwise. For object, ball -> 0 and player -> 1
int triangleEdgeCollisionDetection(int object, double a, double b);
