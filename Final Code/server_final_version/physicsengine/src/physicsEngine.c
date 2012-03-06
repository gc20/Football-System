#include "physicsEngine.h"

// Govind
// Add kick


int main (int argc, const char* argv[]) {
	xilkernel_main();
	return 0;
}


int main_prog(){

	// Default initial values
	xil_printf ("\nInside main_prog physics engine\n");
	gameInPlay = 0;
	gameState = DO_NOTHING;

	//settingUpGame will initialize positions of players to extreme values; speed, direction, kick to null
	xil_printf ("In setting up game\n");
	settingUpGame();

	//initialize interrupts
	xil_printf ("Initialize interrupts \n");
	initializeInterrupts();

	// initialize push buttons
	xil_printf ("Push buttons\n");
	initializePushButtons();

	// initialize mailbox
	xil_printf ("Mailbox\n");
	initializeMailbox();

	//wait for push button to be pressed
	while ((XGpio_DiscreteRead(&GpioInputButton, 1)&1)==0);

	// Receive coordiantes
	xil_printf ("Receive initial data\n");
	receiveInitialData();

	// Wait for 3 seconds before game starts
	sendTime = 0;
	(void) time(&t1);
	prevTime = (long int) (t1);
	while (sendTime <= 3) {
		updateTimer();
		sendViaMailbox();
	}
	gameBit = 1; // Start normal time
	gameInPlay = 1; // Start game

	while (gameInPlay == 1) {

		// Read data obtained from controller
		readQueueData();

		// Run game
		gamePlay();

		// Update timer information
		updateTimer();

		// Spot-refereeing
		if (gameState != FOULALeft && gameState != FOULBLeft && gameState != FOULARight && gameState != FOULBRight)
			checkPushButtonFouls();

		// Prepare structure for sending via mailbox (non-blocking)
		sendViaMailbox();

		// Send update packets to controller
		sendControlPacket();
		sendBallInfoPacket();
		sendPlayerInfoPackets();
	}

	return 1;
}

// Game initialization
void settingUpGame(){

		//generate random positions of players on the left field, used in case of foul on left field
		u8 i;
		for(i = 0; i < 10; i++){
			srand(time(NULL) + i*2);
			playerLeftRandomPos[i].x = 313 - (rand() % 226);
			srand(time(NULL) + i*2 + 1);
			playerLeftRandomPos[i].y = (rand() % 386) + 7;
			//playerLeftRandomPos[i].x = 10 * (i+1);
			//playerLeftRandomPos[i].y = 10 * (i+1);
		}

		//generate random positions of players on the right field, used in case of foul on right field
		for(i = 0; i < 10; i++){
			srand(time(NULL) + i*2);
			playerRightRandomPos[i].x = (rand() % 226) + 327;
			srand(time(NULL) + i*2 + 1);
			playerRightRandomPos[i].y = (rand() % 386) + 7;
			//playerRightRandomPos[i].x = 10 * (i+1);
			//playerRightRandomPos[i].y = 10 * (i+1);
		}

		//generate initial ball position at center of field within a circle of 50 pixel radius. Set speed to 0.
		srand(time(NULL) + 21);
		nike.position.x = (rand()%(2*30) - 30) + 320;
		srand(time(NULL) + 22);
		nike.position.y = (rand()%(2*30) - 30) + 200;
		nike.speed.x = 0;
		nike.speed.y = 0;

		// Set bits such as speed, direction and kick to default value for all players
		for (i=0; i<10; i++) {
			playerAll[i].position.x = 801;
			playerAll[i].position.y = 801;
			playerInitialCoordinates[i].x = 801;
			playerInitialCoordinates[i].y = 801;
			playerAll[i].speed.x = 0;
			playerAll[i].speed.y = 0;
			playerAll[i].direction = 0;
			playerAll[i].kick.speed.x = 0.0;
			playerAll[i].kick.speed.y = 0.0;
			playerAll[i].kick.kickBit = 0;
		}


		// Sample values
		/*playerAll[0].position.x = 34;
		playerAll[0].position.y = 200;
		playerAll[1].position.x = 30;
		playerAll[1].position.y = 350;
		playerAll[2].position.x = 300;
		playerAll[2].position.y = 170;
		playerAll[3].position.x = 300;
		playerAll[3].position.y = 210;
		playerAll[4].position.x = 300;
		playerAll[4].position.y = 250;
		playerAll[5].position.x = 590;
		playerAll[5].position.y = 240;
		playerAll[6].position.x = 590;
		playerAll[6].position.y = 270;
		playerAll[7].position.x = 340;
		playerAll[7].position.y = 170;
		playerAll[8].position.x = 340;
		playerAll[8].position.y = 210;
		playerAll[9].position.x = 340;
		playerAll[9].position.y = 250;
		nike.position.x = 300;
		nike.position.y = 300;
		for (i=5; i<10; i++) {
			playerAll[i].speed.x = 15+2*i * pow (-1, i%2);
			playerAll[i].speed.y = 35-2*i * pow (-1, i%2);
			playerAll[i].direction = i;
			playerAll[i].kick.speed.x = 70.0;
			playerAll[i].kick.speed.y = 70.0;
			playerAll[i].kick.kickBit = 1;
			playerAll[i].kick.speed.x = 40;
			playerAll[i].kick.speed.y = 0;
		}*/
}


//check initial player positions to ensure no overlapping. If overlapped, randomly generate another address
void checkInitialPlayerPos ()
{
	u8 i;

	//check for illegal initial positions
	for(i = 0; i < 10; i++){

		if (playerAll[i].position.x == 801 && playerAll[i].position.y == 801)
			continue;

		//check whether player are out of vertical boundary
		if (playerAll[i].position.x <27 || playerAll[i].position.x > 613) {

			if(i < 5){
				srand(time(NULL));
				playerAll[i].position.x = (rand() % 226) + 77;
			}
			else{
				srand(time(NULL));
				playerAll[i].position.x = (rand() % 226) + 257;
			}
		}

		//check whether players are out of horizontal boundary
		if (playerAll[i].position.y <7 || playerAll[i].position.y > 393) {
			if(i < 5){
				srand(time(NULL));
				playerAll[i].position.y = (rand() % 386) + 7;
			}
			else{
				srand(time(NULL));
				playerAll[i].position.y = (rand() % 386) + 7;
			}
		}


		//checking whether the player is inside the cut-off triangle
		if(triangleEdgeCollisionDetection(1, playerAll[i].position.x, playerAll[i].position.y) >= 1){
				srand(time(NULL));
				playerAll[i].position.x = (rand() % 526) + 47;
				srand(time(NULL));
				playerAll[i].position.y = (rand() % 346) + 7;
		}

	}
}

//***********************************
// communication functions
//***********************************


// Initialize hardware interrupts
void initializeInterrupts () {

	// Initialize DCE interrupts
	int ret, status;
	status = XUartLite_Initialize(&uart_inst, XPAR_RS232_DCE_DEVICE_ID);
	if (status != XST_SUCCESS)
		xil_printf("Failure to initialise!\n"); //XST_FAILURE;

	// Register and enable interrupt handler for UartLite RS232_DTE
	ret = register_int_handler(XPAR_XPS_INTC_1_RS232_DCE_INTERRUPT_INTR, XUartLite_InterruptHandler, (void *)&uart_inst);

	// Registering the receive Xuartlite interrupt handler
	XUartLite_SetRecvHandler(&uart_inst, Recv_HandlerDCE, &uart_inst);

	// Enabling interrupt in the XILKERNEL
	XUartLite_SetSendHandler(&uart_inst, Send_Handler, &uart_inst);

	//Enabling receive handler
	enable_interrupt(XPAR_XPS_INTC_1_RS232_DCE_INTERRUPT_INTR);

	// Enabling interrupt in the Xuartlite hardware i.e. RS232_DTE
	XUartLite_EnableInterrupt(&uart_inst);


	// Initialize DTE interrupts
	status = XUartLite_Initialize(&uart_inst2, XPAR_RS232_DTE_DEVICE_ID);
	if (status != XST_SUCCESS) {
		//xil_printf("Failure to initialise!\n"); //XST_FAILURE;
	}

	// Register and enable interrupt handler for UartLite RS232_DTE
	ret = register_int_handler(XPAR_XPS_INTC_1_RS232_DTE_INTERRUPT_INTR, XUartLite_InterruptHandler, (void *)&uart_inst2);

	// Registering the receive Xuartlite interrupt handler
	XUartLite_SetRecvHandler(&uart_inst2, Recv_HandlerDTE, &uart_inst2);

	// Enabling interrupt in the XILKERNEL
	XUartLite_SetSendHandler(&uart_inst2, Send_Handler, &uart_inst2);

	//Enabling receive handler
	enable_interrupt(XPAR_XPS_INTC_1_RS232_DTE_INTERRUPT_INTR);

	// Enabling interrupt in the Xuartlite hardware i.e. RS232_DTE
	XUartLite_EnableInterrupt(&uart_inst2);

}

//The Receive Interrupt Handler
static void Recv_HandlerDTE (void* CallBackRef, unsigned int EventData)
{
	// Disable interrupts
	disable_interrupt(XPAR_XPS_INTC_1_RS232_DTE_INTERRUPT_INTR);

	// Read data
	//xil_printf("Inside the receive handler\r\n");
    u32 datain, received;
	if((XUartLite_Recv (&uart_inst2, (u8* )&datain, 4))==0) {
	}
	//xil_printf("Not received.\n");
	//xil_printf("Received: %x. %x bytes\n", datain, received);

	// Add data to queue
	qWriteDTE = (qWriteDTE+1)%qSize;
	if (qWriteDTE == qReadDTE)
		qReadDTE = (qReadDTE+1)%qSize;
	circularDataBufferDTE [qWriteDTE] = datain;

	// Enable interrupts
	enable_interrupt(XPAR_XPS_INTC_1_RS232_DTE_INTERRUPT_INTR);
}


//The Receive Interrupt Handler
static void Recv_HandlerDCE (void* CallBackRef, unsigned int EventData)
{
	// Disable interrupts
	disable_interrupt(XPAR_XPS_INTC_1_RS232_DCE_INTERRUPT_INTR);

	// Read data
	//xil_printf("Inside the receive handler\r\n");
    u32 datain, received;
	if((XUartLite_Recv (&uart_inst, (u8* )&datain, 4))==0) {
	}
	//xil_printf("Not received.\n");
	//xil_printf("Received: %x. %x bytes\n", datain, received);

	// Add data to queue
	qWrite = (qWrite+1)%qSize;
	if (qWrite == qRead)
		qRead = (qRead+1)%qSize;
	circularDataBuffer [qWrite] = datain;

	// Enable interrupts
	enable_interrupt(XPAR_XPS_INTC_1_RS232_DCE_INTERRUPT_INTR);
}

// Receive initial coordinates of players
void receiveInitialData () {

	u16 i;
	u8 allPlayerCheck;

	while (1) {
		readQueueData();
		allPlayerCheck = 0;
		for (i=0; i<10; i++) {
			if (playerAll[i].position.x != 801.0 && playerAll[i].position.y != 801.0)
				allPlayerCheck++;
		}

		// Send initial data
		checkInitialPlayerPos();
		sendViaMailbox();

		if (allPlayerCheck == 10)
			break;
	}

	// Store initial data
	//xil_printf ("\nAt store initial data\n");
	for (i=0; i<10; i++) {
		playerInitialCoordinates[i].x = playerAll[i].position.x;
		playerInitialCoordinates[i].y = playerAll[i].position.y;
	}
}


// Read and decode all packets stored in queue buffer
void readQueueData () {

	while (1) {
		if (qRead == qWrite)
			break;
		qRead = (qRead+1)%qSize;
		decodeReceivedPacket(circularDataBuffer[qRead]);
	}
	while(1) {
		if (qReadDTE == qWriteDTE)
			break;
		qReadDTE = (qReadDTE+1)%qSize;
		decodeReceivedPacket(circularDataBufferDTE[qReadDTE]);
	}
}


// Whenever a packet is received, send it to this function, which will decode data appropriately
void decodeReceivedPacket (u32 RecvPacket)
{

	//xil_printf ("\nDecode: Received %d\n", RecvPacket);
	// Extract player index
	u8 playerIndex = ((RecvPacket>>29) & 0x00000001) * 5 + ((RecvPacket>>25) & 0x0000000F);


	// Initial position packet and make sure that game has not started
	if ((RecvPacket>>31) == 1 && gameInPlay == 0) {

		playerAll[playerIndex].position.x = (RecvPacket>>15) & 0x03FF;
		playerAll[playerIndex].position.y = (RecvPacket>>5) & 0x03FF;
	}

	// Update packet
	if ((RecvPacket>>31) == 0) {

		playerAll[playerIndex].kick.kickBit = (RecvPacket>>30) & 0x01;
		u8 direction = (RecvPacket>>21) & 0x0F;
		playerAll[playerIndex].direction = direction;
		u8 speed= ((RecvPacket>>17) & 0x0F);

		if (playerAll[playerIndex].kick.kickBit == 1) {
			if (speed <= 10)
				speed = speed * 5;
			else
				speed = 50 + (speed - 10) * 10;
			playerAll[playerIndex].kick.speed.x = speed * cos(direction*M_PI/8);
			playerAll[playerIndex].kick.speed.y = speed * sin(direction*M_PI/8);
		}
		if (playerAll[playerIndex].kick.kickBit == 0) {
			speed = speed * 5;
			playerAll[playerIndex].speed.x = speed * cos(direction*M_PI/8);
			playerAll[playerIndex].speed.y = speed * sin(direction*M_PI/8);
		}
	}
}


// Send Interrupt Handler (Unused)
static void Send_Handler (void* CallBackRef, unsigned int EventData)
{}


// Send 32-bit packet
void sendPacket (u32 playerPacket) {
	u8 sent = XUartLite_Send(&uart_inst, (u8*)&playerPacket, 4);
	XUartLite_Send(&uart_inst2, (u8*)&playerPacket, 4);
	//xil_printf("Sent: %x. %x bytes\n", playerPacket, sent);
}


// Send control packet
void sendControlPacket ()
{
	// Foul by team A
	if (foulUpdateBit == 1) {
		sendPacket (0xA0000000); // 1010
		foulUpdateBit = 0;
	}

	// Foul by team B
	else if (foulUpdateBit == 2) {
		sendPacket (0xE0000000); // 1110
		foulUpdateBit = 0;
	}

	// Goal by team A
	if (goalUpdateBit == 1) {
		sendPacket (0x80000000); // 1000
		foulUpdateBit = 0;
	}

	// Goal by team B
	else if (goalUpdateBit == 2) {
		sendPacket (0xC0000000); // 1100
		foulUpdateBit = 0;
	}
}

// Send ball info packet
void sendBallInfoPacket()
{

	// Use vx and vy to generate theta and direction
	if (nike.speed.x == 0) {
		if (nike.speed.y < 0)
			nike.direction = 12;
		else
			nike.direction = 4;
	}
	else {
		u8 direction;
		// Find angle
		double angle =  atan(-1 * nike.speed.y/nike.speed.x) / M_PI * 180;
		if (angle < 0)
			angle = angle + 180;
		if (nike.speed.y > 0)
			angle = angle + 180;
		nike.theta = angle;

		// Assign direction based on quadrant
		direction = 0; // Clear direction
		while (!(angle > 78.75 && angle <= 101.25)) {
			direction += 1;
			angle += 22.5;
			if (angle > 360)
				angle -= 360;
		}
		nike.direction = (direction + 12) % 16; // 12 is added to switch from coordinate system in image 1 of wiki to system in image 2
	}

	// Use vx and vy to generate magnitude and direction
	u16 vel = (u16)((nike.speed.y*nike.speed.y) + (nike.speed.x*nike.speed.x));
	u8 speedBall;
	if (vel<6) speedBall = 0x00;
	else if (vel<=56) speedBall = 0x01;
	else if (vel<=156) speedBall = 0x02;
	else if (vel<=306) speedBall = 0x03;
	else if (vel<=506) speedBall = 0x04;
	else if (vel<=756) speedBall = 0x05;
	else if (vel<=1056) speedBall = 0x06;
	else if (vel<=1406) speedBall = 0x07;
	else if (vel<=1806) speedBall = 0x08;
	else if (vel<=2256) speedBall = 0x09;
	else if (vel<=3025) speedBall = 0x0A;
	else if (vel<=4225) speedBall = 0x0B;
	else if (vel<=5625) speedBall = 0x0C;
	else if (vel<=7225) speedBall = 0x0D;
	else if (vel<=9025) speedBall = 0x0E;
	else if (vel>9025) speedBall = 0x0F;

	// Generate packet
	u32 ballPacket=0x00000000;
	u32 tempPacket = 0x00000000;
	// embed ball direction
	tempPacket = nike.direction;
	tempPacket = (tempPacket<<25) & 0x1e000000;
	ballPacket = ballPacket | tempPacket;
	// embed ball x position
	tempPacket = (u16)nike.position.x;
	tempPacket = (tempPacket<<15) & 0x01ff8000;
	ballPacket = ballPacket | tempPacket;
	// embed ball y position
	tempPacket = (u16)nike.position.y;
	tempPacket = (tempPacket<<5) & 0x00007fe0;
	ballPacket = ballPacket | tempPacket;
	// embed ball speed
	tempPacket = speedBall;
	tempPacket = (tempPacket<<1) & 0x0000001e;
	ballPacket = ballPacket | tempPacket;

	// Send packet
	sendPacket (ballPacket);
}


// Send player info packets
void sendPlayerInfoPackets ()
{
	// Variables
	u8 i;
	u32 playerPacket;
	u32 tempPacket = 0x00000000;

	// Send info of each player
	for(i=0; i<10; i++)
	{
		// Embed team number and player ID
		tempPacket = i;
		if (i < 5) {
			playerPacket = 0x20000000;
		}
		else {
			playerPacket = 0x60000000;
			tempPacket = tempPacket-5;
		}
		tempPacket = (tempPacket<<25)& 0x1e000000;
		playerPacket = playerPacket | tempPacket;

		// Embed x position
		tempPacket = (u16)playerAll[i].position.x;
		tempPacket = (tempPacket<<15)& 0x01ff8000;
		playerPacket = playerPacket | tempPacket;

		// Embed y postion
		tempPacket = (u16)playerAll[i].position.y;
		tempPacket = (tempPacket<<5)& 0x00007fe0;
		playerPacket = playerPacket | tempPacket;

		// Send the packet
		sendPacket(playerPacket);
	}
}

// Setup mailboxes
void initializeMailbox() {

	XStatus status;
	ConfigPtr = XMbox_LookupConfig(XPAR_MBOX_0_DEVICE_ID );
	if (ConfigPtr == (XMbox_Config *)NULL){
		//xil_printf ("\t Mailbox LookupConfig Failed.\r\n");
		//return XST_FAILURE;
	}

	status = XMbox_CfgInitialize(&mbox, ConfigPtr, ConfigPtr->BaseAddress);
	if (status != XST_SUCCESS){
		//xil_printf ("\tMailbox XMbox_CfgInitialize Failed.\r\n");
		//return status;
	}
}


// Send information via mailbox
void sendViaMailbox() {

	// Prepare structure
	mailboxSendData.player0x = playerAll[0].position.x;
	mailboxSendData.player1x = playerAll[1].position.x;
	mailboxSendData.player2x = playerAll[2].position.x;
	mailboxSendData.player3x = playerAll[3].position.x;
	mailboxSendData.player4x = playerAll[4].position.x;
	mailboxSendData.player5x = playerAll[5].position.x;
	mailboxSendData.player6x = playerAll[6].position.x;
	mailboxSendData.player7x = playerAll[7].position.x;
	mailboxSendData.player8x = playerAll[8].position.x;
	mailboxSendData.player9x = playerAll[9].position.x;
	mailboxSendData.player0y = playerAll[0].position.y;
	mailboxSendData.player1y = playerAll[1].position.y;
	mailboxSendData.player2y = playerAll[2].position.y;
	mailboxSendData.player3y = playerAll[3].position.y;
	mailboxSendData.player4y = playerAll[4].position.y;
	mailboxSendData.player5y = playerAll[5].position.y;
	mailboxSendData.player6y = playerAll[6].position.y;
	mailboxSendData.player7y = playerAll[7].position.y;
	mailboxSendData.player8y = playerAll[8].position.y;
	mailboxSendData.player9y = playerAll[9].position.y;
	mailboxSendData.ballx = nike.position.x;
	mailboxSendData.bally = nike.position.y;
	mailboxSendData.goalA = goalA;
	mailboxSendData.goalB = goalB;
	mailboxSendData.foulA = foulA;
	mailboxSendData.foulB = foulB;
	mailboxSendData.time = sendTime;

	// Non-blocking
	/*int sent = 0, nbytes = 0, ret;
	while (nbytes < sizeof(mailboxSendData)) {
		//Read a message from the mbox
	    (void)XMbox_Write (&mbox, (u32)(&mailboxSendData + nbytes), sizeof(mailboxSendData) - nbytes, &sent);
	    nbytes += sent;
		 if(ret!=0) {
			 ////xil_printf("\rERROR!! could not send data ");
			 break;
		 }
	}*/

	// Blocking
	////xil_printf ("\nSending via mailbox");
	XMbox_WriteBlocking(&mbox, (u32)(&mailboxSendData), sizeof(mailboxSendData));

}


// Initialize push buttons
void initializePushButtons () {

	// Initialize buttons
    if (GpioInputInitialize(XPAR_BUTTONS_3BIT_DEVICE_ID, &GpioInputButton) == 1) {
    	//xil_printf("GpioInput Buttons FAILED.\r\n");
    }
}


// Read GPIO input
int GpioInputInitialize(u32 DeviceId, XGpio *GpioInput) {

	// Initialize GPIO instance
	if (XGpio_Initialize(GpioInput, DeviceId) != XST_SUCCESS) {
		return XST_FAILURE;
	}

	// Set the direction for all signals to be inputs
	XGpio_SetDataDirection(GpioInput, 1, 0xFFFFFFFF);

	return XST_SUCCESS;
}


// Check if push button has been pressed to indicate foul
void checkPushButtonFouls () {

	u8 buttons = XGpio_DiscreteRead(&GpioInputButton, 1);

	// Foul by team A (East D18)
	if(((buttons/2)&1) == 1){
		foulA++;
		gameState = FOULALeft;

		/*// Wait for button to be unpressed
		sendViaMailbox();
		buttons = XGpio_DiscreteRead(&GpioInputButton, 1);
		while (((buttons/2)&1) == 1)
		{}*/

	}
	// Foul by team B (West H13)
	else if(((buttons/4)&1) == 1){
		foulB++;
		gameState = FOULBRight;

		// Wait for button to be unpressed
		/*sendViaMailbox();
		buttons = XGpio_DiscreteRead(&GpioInputButton, 1);
		while (((buttons/4)&1) == 1)
		{}*/
	}

}

// Update timer count
void updateTimer ()
{
	(void) time(&t1);
	sendTime = (int) (t1 - prevTime);
	//xil_printf ("\n%d\n", sendTime);
}


// check for foul and goal
void checkFoulGoalConditions()
{

	//Check for scoring of goal on the left side by the right side. NOTE:Goal is scored when whole ball in.
	if(nike.position.x <= 15){
		//xil_printf ("\n GOAL B\n");
		if (gameState != GOAL) {
			goalB = goalB+1;
			gameState = GOAL;
		}
		nike.speed.x = 0;
		nike.speed.y = 0;
	}
	//Check for scoring of goal on the right side by the left side
	if(nike.position.x >= 625){
		//xil_printf ("\n GOAL A\n");
		if (gameState != GOAL) {
			goalA = goalA+1;
			gameState = GOAL;
		}
		nike.speed.x = 0;
		nike.speed.y = 0;
 	}

	//Check for fouls.
	int countA, countB;
	u8 i;
	if(!(nike.position.x >= 25 && nike.position.x <= 65 && nike.position.y >= 165 && nike.position.y <= 235)){
		countA = 0; countB = 0;
		for(i = 0; i < 10; i++){
			if(playerAll[i].position.x >= 27 && playerAll[i].position.x <= 63 &&
			   playerAll[i].position.y >= 167 && playerAll[i].position.y <= 233){
					if(i < 5) countA++;
					else countB++;
			   }
		}
		if(countA > 2){
			//xil_printf ("\n FoulALeft\n");
			foulA=foulA+1;
			gameState = FOULALeft;
		}
		else if(countB > 1){
			//xil_printf ("\n FoulBLeft\n");
			foulB=foulB+1;
			gameState = FOULBLeft;
		}
	}


	if(!(nike.position.x >= 575 && nike.position.x <= 615 && nike.position.y >= 165 && nike.position.y <= 235)){
		countA = 0; countB = 0;
		for(i = 0; i < 10; i++){
			if(playerAll[i].position.x >= 557 && playerAll[i].position.x <= 593 &&
			   playerAll[i].position.y >= 167 && playerAll[i].position.y <= 233){
					if(i < 5) countA++;
					else countB++;
			   }
		}
		if(countA > 1){
			//xil_printf ("\n FoulARight\n");
			foulA++;
			gameState = FOULARight;
		}
		else if(countB > 2){
			//xil_printf ("\n FoulBRight\n");
			foulB++;
			gameState = FOULBRight;
		}
	}

}

// Update speeds of ball and players
void calculateNewSpeeds() {


	// Update player speeds
	u8 i;
	//xil_printf ("directions: ");
	for (i=0; i<10; i++) {
		playerAll[i].position.x = ((double)(playerAll[i].position.x)) + playerAll[i].speed.x * assumedPeriod;
		playerAll[i].position.y = ((double)(playerAll[i].position.y)) + playerAll[i].speed.y * assumedPeriod;
		//xil_printf ("%d", playerAll[i].direction);
	}
	//xil_printf ("\n");

	// 20 -> no player has kicked
	if (playerToKick == 20) {

		nike.position.x = ((double)(nike.position.x)) + nike.speed.x * assumedPeriod;
		nike.position.y = ((double)(nike.position.y)) + nike.speed.y * assumedPeriod;
		// Account for friction
		if (nike.speed.x < 0) {
			nike.speed.x += 10.0*assumedPeriod;
			if (nike.speed.x > 0)
				nike.speed.x = 0;
		}
		else {
			nike.speed.x -= 10.0*assumedPeriod;
			if (nike.speed.x < 0)
				nike.speed.x = 0;
		}
		if (nike.speed.y < 0) {
			nike.speed.y += 10.0*assumedPeriod;
			if (nike.speed.y > 0)
				nike.speed.y = 0;
		}
		else {
			nike.speed.y -= 10.0*assumedPeriod;
			if (nike.speed.y < 0)
				nike.speed.y = 0;
		}
	}
	else {
		//xil_printf ("\n %d has kicked\n", playerToKick);
		//Somebody has kicked the ball, the ball gets the velocity of the player
		if (playerToKick >= 0 && playerToKick <= 9) {
			nike.speed.x = playerAll[playerToKick].kick.speed.x;
			nike.speed.y = playerAll[playerToKick].kick.speed.y;
			nike.position.x = ((double)(nike.position.x)) + nike.speed.x * assumedPeriod;
			nike.position.y = ((double)(nike.position.y)) + nike.speed.y * assumedPeriod;
			playerAll[playerToKick].position.x = ((double)(playerAll[playerToKick].position.x)) - playerAll[playerToKick].speed.x * assumedPeriod;
			playerAll[playerToKick].position.y = ((double)(playerAll[playerToKick].position.y)) - playerAll[playerToKick].speed.y * assumedPeriod;
		}
	}
	playerToKick = 20;

}


// Check if player's kick is directionally valid
u8 checkBallInFront (u8 i)
{

	//find the angle between the line connecting ball with player and x-axis,
	//and the angle between player velocity in x-direction and y-direction

	int dx = nike.position.x - playerAll[i].position.x;
	int dy = -1 *(nike.position.y - playerAll[i].position.y);
	int kickDirection = 0;
	double angle;

	// Check for special conditions where tan function will be undefined
	if (dx == 0) {
		if (dy>0)
			kickDirection = 12;
		else
			kickDirection = 4;
	}
	else if (dy == 0) {
		if (dx>0)
			kickDirection = 0;
		else
			kickDirection = 8;
	}

	// Find direction iteratively for the generic scenario
	else {

		// Find angle of rotation
		angle = atan ((double)dy/(double)dx) / M_PI * 180;
		if (angle < 0)
			angle = angle + 180;
		if (dy < 0)
			angle = angle + 180;

		// Assign direction based on quadrant
		kickDirection = 0; // Clear direction
		while (!(angle > 78.75 && angle <= 101.25)) {
			kickDirection += 1;
			angle += 22.5;
			if (angle > 360)
				angle -= 360;
		}
		kickDirection = (kickDirection + 12) % 16; // 12 is added to switch from coordinate system in image 1 of wiki to system in image 2
	}

	// Check if kick is valid
	u8 j;
	for (j=0; j<=4; j++) {
		if ( ((kickDirection+j)%16 == playerAll[i].direction) || ((kickDirection-j)%16 == playerAll[i].direction)) {
			//xil_printf ("\nKick in front!\n");
			return ((u8)1);
		}
	}
	//xil_printf ("\nKick not in front! %d \n", playerAll[i].direction);
	return ((u8)0);
}


// Check if player is allowed to kick
void checkPlayerToKick ()
{

	double minDistancePlayerBall = 0;
	double tempDistancePlayerBall = 0;
	u8 i;
	playerToKick = 20;

	//xil_printf ("\n");
	//Determine who is eligible to kick
	for(i = 0; i < 10; i++){

		tempDistancePlayerBall = computeDistance(playerAll[i].position.x, playerAll[i].position.y, nike.position.x, nike.position.y);
		//xil_printf ("%f %d %f %f %f %f \n", tempDistancePlayerBall, i, playerAll[i].position.x, playerAll[i].position.y, nike.position.x, nike.position.y);
		//if the player has kicked, and the NON-OVERLAPPING DISTANCE BWTWEEN HIM AND BALL IS LESS THAN 3 PIXELS
		if (tempDistancePlayerBall <= 225.0){
	//playerAll[i].kick.kickBit == 1 &&
			//check whether the ball is in front of player
			if(checkBallInFront(i) == 1){
				if (playerToKick == 20 || tempDistancePlayerBall < minDistancePlayerBall){
					minDistancePlayerBall = tempDistancePlayerBall;
					playerToKick = i;
				}
			}
		}
	}
}

// Run the game
void gamePlay ()
{

	switch (gameState) {

		case DO_NOTHING: {
			if (gameBit == 1) {
				countdownLimit = 299; // 5 minutes
				gameState = NORMAL;
			}
			else if (gameBit == 3) {
				sendTime = 0;
				(void) time(&t1);
				prevTime = (long int) (t1);
				countdownLimit = 179; // 3 minutes
				gameState = NORMAL;
			}
			else if (gameBit == 2 || gameBit == 4) {
				sleep (1000); // Wait for 1 second
				gameState = END_GAME;
			}
			break;
		}

		case NORMAL: {

			//xil_printf ("\n Send time: %d\n", sendTime);
			// updates positions based on speed
			updateBallPlayerPos();

			// Check timer
			if (sendTime > countdownLimit) {
				if (gameBit == 1)
					gameBit = 2;
				if (gameBit == 3)
					gameBit = 4;
				gameState = DO_NOTHING;
			}
			break;
		}

		case END_GAME: {

			// Check if game is over
			if(goalA > goalB){
				winnerBit = 1;
				gameInPlay = 0;
			}
			else if(goalA < goalB){
				winnerBit = 2;
				gameInPlay = 0;
			}
			else  {

				// Check if its the end of normal time
				if (gameBit == 2) {
					winnerBit = 3;
					gameBit = 3;
					gameState = DO_NOTHING;
				}
				else {
					if (foulA < foulB)
						winnerBit = 1;
					else if(foulA > foulB)
						winnerBit = 2;
					else
						winnerBit = 4;
					gameInPlay = 0;
				}
			}
			break;
		}

		case GOAL: {

			// If goal has just been scored
			if (goalTime == 100) {
				//xil_printf ("\n Goal has now been recorded\");
				goalTime = sendTime;
			}

			// Update position
			updateBallPlayerPos();

			// Check if goal condition is done after 2 seconds
			if ((sendTime-goalTime) >= 2) {

				//xil_printf ("\nGoal time done\n");
				goalTime = 100;
				u8 i;
				// Set bits such as speed, direction and kick to default value for all players. also push them to initial positions.
				for (i=0; i<10; i++) {
					playerAll[i].position.x = playerInitialCoordinates[i].x;
					playerAll[i].position.y = playerInitialCoordinates[i].y;
					playerAll[i].speed.x = 0;
					playerAll[i].speed.y = 0;
					playerAll[i].kick.speed.x = 0.0;
					playerAll[i].kick.speed.y = 0.0;
					playerAll[i].kick.kickBit = 0;
					playerAll[i].direction = 0;
					if (i >= 5)
						playerAll[i].direction = 8;

				}
				nike.speed.x = 0;
				nike.speed.y = 0;
				srand (time(NULL));
				nike.position.x = (rand()%(2*30) - 30) + 320;
				srand (time(NULL) + 1);
				nike.position.y = (rand()%(2*30) - 30) + 200;
				gameState = NORMAL;
			}
			break;
		}


		case FOULALeft: {

			sleep(1000);
			// We have to move all the players to the half where foul is committed, and place a player from the non-foul team next to ball
			u8 i;
			for(i = 0; i < 10; i++){
				playerAll[i].position.x = playerLeftRandomPos[i].x;
				playerAll[i].position.y = playerLeftRandomPos[i].y;
			}

			// Send nike positions
			nike.position.x = 165;
			nike.position.y = 200;

			// Move one player close to the ball
			playerAll[6].position.x = 180;
			playerAll[6].position.y = 200;

			gameState = NORMAL;
			break;
		}

		case FOULBLeft: {

			sleep(1000);
			// We have to move all the players to the half where foul is committed, and place a player from the non-foul team next to ball
			u8 i;
			for(i = 0; i < 10; i++){
				playerAll[i].position.x = playerLeftRandomPos[i].x;
				playerAll[i].position.y = playerLeftRandomPos[i].y;
			}

			// Send nike positions
			nike.position.x = 165;
			nike.position.y = 200;

			// Move one player close to the ball
			playerAll[1].position.x = 150;
			playerAll[1].position.y = 200;

			gameState = NORMAL;
			break;
		}


		case FOULARight: {

			sleep(1000);
			// We have to move all the players to the half where foul is committed, and place a player from the non-foul team next to ball
			u8 i;
			for(i = 0; i < 10; i++){
				playerAll[i].position.x = playerRightRandomPos[i].x;
				playerAll[i].position.y = playerRightRandomPos[i].y;
			}

			// Send nike positions
			nike.position.x = 465;
			nike.position.y = 200;

			// Move one player close to the ball
			playerAll[6].position.x = 480;
			playerAll[6].position.y = 200;

			gameState = NORMAL;
			break;
		}

		case FOULBRight: {

			sleep(1000);
			// We have to move all the players to the half where foul is committed, and place a player from the non-foul team next to ball
			u8 i;
			for(i = 0; i < 10; i++){
				playerAll[i].position.x = playerRightRandomPos[i].x;
				playerAll[i].position.y = playerRightRandomPos[i].y;
			}

			// Send nike positions
			nike.position.x = 465;
			nike.position.y = 200;

			// Move one player close to the ball
			playerAll[1].position.x = 450;
			playerAll[1].position.y = 200;

			gameState = NORMAL;
			break;
		}

		default:
			break;
	} // switch end

}


// Function which handles the core execution of the game
void updateBallPlayerPos() {

	checkSpeedLegality();
	checkPlayerToKick();
	calculateNewSpeeds();
	checkBallColPlayer();
	checkPlayerColPlayer();
	checkBallColWall();
	checkPlayerColWall();
	checkSpeedLegality();
	checkFoulGoalConditions();
}


// Compute distance between two positions
double computeDistance(double x1, double y1, double x2, double y2)
{
	double d;
	d = ((x2-x1) * (x2-x1) + (y2-y1) * (y2-y1));
	//xil_printf("dist %f\n",d);
	return d;
}

// Check if provided speed is legal
void checkSpeedLegality () {

	u8 i;
	// Check for illegal speed, and set speed to 0 if illegal. Also check for illegal direction
	for (i=0; i<10; i++) {
		if (((playerAll[i].speed.x * playerAll[i].speed.x) + (playerAll[i].speed.y * playerAll[i].speed.y)) > 2500) {
			playerAll[i].speed.x = 0;
			playerAll[i].speed.y = 0;
		}
		playerAll[i].direction = playerAll[i].direction % 16;
	}

	// Ensure that ball is within legal limits
	if (((nike.speed.x * nike.speed.x) + (nike.speed.y * nike.speed.y)) > 10000) {
		double mag = sqrt ((nike.speed.x * nike.speed.x) + (nike.speed.y * nike.speed.y));
		nike.speed.x = nike.speed.x / mag * 100;
		nike.speed.y = nike.speed.y / mag * 100;
	}

	if (nike.position.x <= 10 || nike.position.x >= 630){
		nike.speed.x = 0;
		nike.speed.y = 0;
	}
}

// Collision Case 1: Check whether ball and player have collided
void checkBallColPlayer() {

	int distanceBallPlayer;
	double theta;
	double tempSpeedVa1, tempSpeedVb1, sinTheta, cosTheta, temp;
	int dx, dy;
	u8 i, j, k;

	for(i = 0; i < 10; i++){

			// Find distance between the two players
			distanceBallPlayer = computeDistance(nike.position.x, nike.position.y, playerAll[i].position.x, playerAll[i].position.y);

			// Check if overlap occurs
			if (distanceBallPlayer <= 144){

				// Unoverlap
				/*for (k=1; k<=10; k++) {
					playerAll[i].position.x -= playerAll[i].speed.x*assumedPeriod * granularity;
					playerAll[i].position.y -= playerAll[i].speed.y*assumedPeriod * granularity;
					nike.position.x -= nike.speed.x*assumedPeriod * granularity;
					nike.position.y -= nike.speed.y*assumedPeriod * granularity;
					if (computeDistance(nike.position.x, nike.position.y, playerAll[i].position.x, playerAll[i].position.y) >= 144)
						break;
				}*/

				// Unoverlap
				playerAll[i].position.x -= playerAll[i].speed.x * assumedPeriod;
				playerAll[i].position.y -= playerAll[i].speed.y * assumedPeriod;

				// Find angle between ball and player
				dx = nike.position.x - playerAll[i].position.x;
				dy = -1 * (nike.position.y - playerAll[i].position.y);

				// Check for special conditions where tan function will be undefined
				if (dx == 0) {
					nike.speed.y *= -1;
				}
				else if (dy == 0) {
					nike.speed.x *= -1;
				}
				else {
					// Find angle of rotation
					theta = atan ((double)dy/(double)dx) / M_PI * 180;
					if (theta < 0)
						theta = theta + 180;
					if (dy < 0)
						theta = theta + 180;
					while (theta > 180)
						theta = theta - 180;

					// Reverse collision direction
					sinTheta = sin(theta * M_PI / 180);
					cosTheta = cos(theta * M_PI / 180);
					tempSpeedVa1 = (nike.speed.x * cosTheta + nike.speed.y * sinTheta) * -1;
					tempSpeedVb1 = nike.speed.x * sinTheta - nike.speed.y * cosTheta;
					nike.speed.x = tempSpeedVa1 * cosTheta - tempSpeedVb1 * sinTheta;//+
					nike.speed.y = tempSpeedVa1 * sinTheta + tempSpeedVb1 * cosTheta;//-
				}
		}
	}
}

// Collision Case 2: check whether player and player have collided
void checkPlayerColPlayer() {

	int distancePlayerPlayer;
	double theta;
	double tempSpeedVa1, tempSpeedVa2, tempSpeedVb1, tempSpeedVb2, sinTheta, cosTheta, temp;
	int dx, dy;
	u8 i, j, k;

	for(i = 0; i < 10; i++){
		for(j = i+1; j < 10; j++){

			// Find distance between the two players
			distancePlayerPlayer = computeDistance(playerAll[i].position.x, playerAll[i].position.y,
					playerAll[j].position.x, playerAll[j].position.y);

			// Check if overlap occurs
			if (distancePlayerPlayer <= 196) {

				// Unoverlap
				for (k=1; k<=10; k++) {
					playerAll[i].position.x -= playerAll[i].speed.x*assumedPeriod * granularity;
					playerAll[i].position.y -= playerAll[i].speed.y*assumedPeriod * granularity;
					playerAll[j].position.x -= playerAll[j].speed.x*assumedPeriod * granularity;
					playerAll[j].position.y -= playerAll[j].speed.y*assumedPeriod * granularity;
					if (computeDistance(playerAll[i].position.x, playerAll[i].position.y, playerAll[j].position.x, playerAll[j].position.y) >= 196)
						break;
				}

				// Find angle between players
				dx = playerAll[i].position.x - playerAll[j].position.x;
				dy = -1 * (playerAll[i].position.y - playerAll[j].position.y);

				// Check for special conditions where tan function will be undefined
				if (dx == 0) {
					temp = playerAll[i].speed.y;
					playerAll[i].speed.y = playerAll[j].speed.y;
					playerAll[j].speed.y = temp;
				}
				else if (dy == 0) {
					temp = playerAll[i].speed.x;
					playerAll[i].speed.x = playerAll[j].speed.x;
					playerAll[j].speed.x = temp;
				}
				else {
					// Find angle of rotation
					theta = atan ((double)dy/(double)dx) / M_PI * 180;
					if (theta < 0)
						theta = theta + 180;
					if (dy < 0)
						theta = theta + 180;
					while (theta > 180)
						theta = theta - 180;

					/*// Player i
					tempSpeedVp = playerAll[i].speed.x * sin (theta) - playerAll[i].speed.y * cos (theta);
					playerAll[i].speed.x = tempSpeedVp * sin (theta);
					playerAll[i].speed.y = -1 * tempSpeedVp * cos (theta);

					// Player j
					tempSpeedVp = playerAll[j].speed.x * sin (theta) - playerAll[j].speed.y * cos (theta);
					playerAll[j].speed.x = tempSpeedVp * sin (theta);
					playerAll[j].speed.y = -1 * tempSpeedVp * cos (theta);*/

					// General equations
					sinTheta = sin(theta * M_PI / 180);
					cosTheta = cos(theta * M_PI / 180);
					tempSpeedVa1 = playerAll[i].speed.x * cosTheta + playerAll[i].speed.y * sinTheta;
					tempSpeedVb1 = playerAll[i].speed.x * sinTheta - playerAll[i].speed.y * cosTheta;
					tempSpeedVa2 = playerAll[j].speed.x * cosTheta + playerAll[j].speed.y * sinTheta;
					tempSpeedVb2 = playerAll[j].speed.x * sinTheta - playerAll[j].speed.y * cosTheta;

					// Player i
					playerAll[i].speed.x = tempSpeedVa2 * cosTheta - tempSpeedVb1 * sinTheta;//+
					playerAll[i].speed.y = tempSpeedVa2 * sinTheta + tempSpeedVb1 * cosTheta;//-

					// Player j
					playerAll[j].speed.x = tempSpeedVa1 * cosTheta - tempSpeedVb2 * sinTheta;//+
					playerAll[j].speed.y = tempSpeedVa1 * sinTheta + tempSpeedVb2 * cosTheta;//-
				}
			}
		}
	}
}


// When ball collides with wall
void checkBallColWall()
{

	u8 j;
	u8 check = 0;
	u8 triangleCondition;

	//when ball collides with vertical wall, update speed, x direction velocity reversed, y direction velocity unchanged
	if(nike.position.x < (20+5)){
		check = 1;
		if (!(nike.position.y >= 175 && nike.position.y <= 225))
			if (nike.speed.x < 0)
				nike.speed.x = nike.speed.x * -1;
   	}
	else if(nike.position.x > (620-5)){
		check = 1;
		if (!(nike.position.y >= 175 && nike.position.y <= 225))
			if (nike.speed.x > 0)
				nike.speed.x = nike.speed.x * -1;
   	}

	//when ball collides with horizontal wall, update speed, y direction velocity reversed, x direction velocity unchanged
	if((nike.position.y < (0+5)) && (nike.speed.y < 0)) {
		check = 1;
		nike.speed.y = nike.speed.y * -1;
	}
	else if((nike.position.y > (400-5)) && (nike.speed.y > 0)) {
		check = 1;
		nike.speed.y = nike.speed.y * -1;
	}

	//when collided with triangles cut-off at four corners
	triangleCondition = triangleEdgeCollisionDetection(0, nike.position.x, nike.position.y);
	if (check == 0 && triangleCondition >= 1) {// Unoverlap

		// Unoverlap
		for (j=0; j<10; j++) {
			nike.position.x -= nike.speed.x * assumedPeriod * granularity;
			nike.position.y -= nike.speed.y * assumedPeriod * granularity;
			if ((triangleEdgeCollisionDetection(0, nike.position.x, nike.position.y) == 0))
				break;
		}

		// Check 4 conditions
		if (triangleCondition == 1) {
			nike.position.x = 33;
			nike.position.y = 13;
		}
		else if (triangleCondition == 2) {
			nike.position.x = 607;
			nike.position.y = 13;
		}
		else if (triangleCondition == 3) {
			nike.position.x = 33;
			nike.position.y = 387;
		}
		else if (triangleCondition == 4) {
			nike.position.x = 607;
			nike.position.y = 387;
		}

		// Determine speed
		tempSwap = nike.speed.x;
		nike.speed.x = -1 * nike.speed.y;
		nike.speed.y = -1 * tempSwap;
    }

    // Just in case, check for special conditions
    if (nike.position.x < 25) {
       if (nike.position.y <= 225 && nike.position.y >= 175) {
    	   if (nike.position.x < 5)
    		   nike.position.x = 5;
       }
       else {
    	   nike.position.x = 25;
       }
    }
    if (nike.position.x > 615) {
       if (nike.position.y <= 225 && nike.position.y >= 175) {
    	   if (nike.position.x > 635)
    		   nike.position.x = 635;
       }
       else {
    	   nike.position.x = 615;
       }
    }
    if (nike.position.y < 5) {
       nike.position.y = 5;
    }
    if (nike.position.y > 395) {
       nike.position.y = 395;
    }
}


// When player collides with wall
void checkPlayerColWall()
{
	u8 i, j, triangleCondition;
	for (i=0; i<10; i++) {
		//when ball collides with vertical wall, update speed, x direction velocity reversed, y direction velocity unchanged
		if(playerAll[i].position.x < (20+7)){
			//if (!(playerAll[i].position.y >= 177 && playerAll[i].position.y <= 223))
				if (playerAll[i].speed.x < 0)
					playerAll[i].speed.x = playerAll[i].speed.x * -1;
		}
		else if(playerAll[i].position.x > (620-7)){
			//if (!(playerAll[i].position.y >= 177 && playerAll[i].position.y <= 223))
				if (playerAll[i].speed.x > 0)
					playerAll[i].speed.x = playerAll[i].speed.x * -1;
		}

		//when ball collides with horizontal wall, update speed, y direction velocity reversed, x direction velocity unchanged
		if((playerAll[i].position.y < (0+7)) && (playerAll[i].speed.y < 0)) {
			playerAll[i].speed.y = playerAll[i].speed.y * -1;
		}
		else if((playerAll[i].position.y > (400-7)) && (playerAll[i].speed.y > 0)) {
			playerAll[i].speed.y = playerAll[i].speed.y * -1;
		}

		//when collided with triangles cut-off at four corners
		triangleCondition = triangleEdgeCollisionDetection(1, playerAll[i].position.x, playerAll[i].position.y);
		if (triangleCondition >= 1) {

			// Unoverlap
			for (j=0; j<10; j++) {
				playerAll[i].position.x -= playerAll[i].speed.x * assumedPeriod * granularity; // 0.2 = granularity
				playerAll[i].position.y -= playerAll[i].speed.y * assumedPeriod * granularity; // 0.2 = granularity
				if ((triangleEdgeCollisionDetection(1, playerAll[i].position.x, playerAll[i].position.y) == 0))
					break;
			}

			// Check 4 conditions
			if (triangleCondition == 1) {
				playerAll[i].position.x = 35;
				playerAll[i].position.y = 15;
			}
			else if (triangleCondition == 2) {
				playerAll[i].position.x = 605;
				playerAll[i].position.y = 15;
			}
			else if (triangleCondition == 3) {
				playerAll[i].position.x = 35;
				playerAll[i].position.y = 385;
			}
			else if (triangleCondition == 4) {
				playerAll[i].position.x = 605;
				playerAll[i].position.y = 385;
			}

			// Determine speed
			tempSwap = playerAll[i].speed.x;
			playerAll[i].speed.x = -1 * playerAll[i].speed.y;
			playerAll[i].speed.y = -1 * tempSwap;
		}

		// Just in case, check for special conditions
		if (playerAll[i].position.x < 27) {
			playerAll[i].position.x = 27;
		}
		if (playerAll[i].position.x > 613) {
			playerAll[i].position.x = 613;
		}
		if (playerAll[i].position.y < 7) {
			playerAll[i].position.y = 7;
		}
		if (playerAll[i].position.y > 393) {
			playerAll[i].position.y = 393;
		}
	}
}


// returns 1 is there is collision, 0 otherwise. For object, ball -> 0 and player -> 1
int triangleEdgeCollisionDetection(int object, double a, double b) {

	if(object == 1){
		if ((a+b) < 50)
			return 1;
		if ((a-b) > 590)
			return 2;
		if ((b-a) > 350)
			return 3;
		if ((a+b) > 990)
			return 4;
	}
	else{
		if ((a+b) < 47)
			return 1;
		if ((a-b) > 593)
			return 2;
		if ((b-a) > 353)
			return 3;
		if ((a+b) > 993)
			return 4;
	}
	return 0;
}
/*int triangleEdgeCollisionDetection(int object, double a, double b) {

	return 0;
	double constant = 50.0;
	if(object == 1)
		constant = 98.0;

	if ((b + a - 40.0)*(b + a - 40.0) <= constant)		// edge 1 Top-Left
		return 1;
	if ((a - b - 600.0)*(a - b - 600.0) <= constant)	// edge 2 Top-Right
		return 2;
	if ((b - a - 360.0)*(b - a - 360.0) <= constant)	// edge 3 Bottom-Left
		return 3;
	if ((b + a - 1000.0)*(b + a - 1000.0) <= constant)	// edge 4 Bottom-Right
		return 4;

	return 0;
}*/
