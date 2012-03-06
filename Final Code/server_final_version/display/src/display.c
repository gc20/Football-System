#include "xparameters.h"
#include "xtft.h"
#include "xmbox.h"
#define TFT_DEVICE_ID XPAR_XPS_TFT_0_DEVICE_ID
#define MSGSIZ  27
#define PCK_SIZE 108
int rcvmsg[MSGSIZ];
XMbox mbox;
XMbox_Config *ConfigPtr;
XStatus status;
static XTft TftInstance;
typedef struct position {
    int x, y;
} position;
typedef struct
{
	int goalA, goalB, foulA, foulB;
}score;
typedef struct
{
	int width, height;
}size;
typedef struct


{
	int pixel[2][69];
}ballpixelarray;
typedef struct
{
	int pixel[2][145];
}playerpixelarray;
typedef struct
{
	int counter;
	int minutes;
	int seconds;
	char min;
	char sec_1;
	char sec_2;
}clocking;
//------------------------------------Global Variables ----------------------------------------//
position nike;
position prevnike_2;
position prevnike_3;
position prevplayerAll_2[10];
position prevplayerAll_3[10];
position playerAll[10];
u32 ballColBuf_2[69] = {0};
u32 ballColBuf_3[69] = {0};
ballpixelarray barray;
playerpixelarray parray;
u32 playerColBuf_2[10][145] = {0};
u32 playerColBuf_3[10][145] = {0};
score s;
clocking projectTimer;
u8 checkplayers = 1;
u32 vdm1;
u32 vdm2 = 0xC7C00000;
u32 vdm3 = 0xC7A00000;
u8 pass1 = 0,pass2 = 0;
//------------------------------------Global Variables ----------------------------------------//
void initializeMailbox() {
	XStatus status;
	ConfigPtr = XMbox_LookupConfig(XPAR_MBOX_0_DEVICE_ID );
	if (ConfigPtr == (XMbox_Config *)NULL){
		return XST_FAILURE;
	}
	status = XMbox_CfgInitialize(&mbox, ConfigPtr, ConfigPtr->BaseAddress);
	if (status != XST_SUCCESS){
		return status;
	}
}
void* recv ()
{
	int ret;
	(void)XMbox_ReadBlocking(&mbox, (u32*)(&rcvmsg),PCK_SIZE);
	playerAll[0].x = rcvmsg[0];
	playerAll[1].x = rcvmsg[1];
	playerAll[2].x = rcvmsg[2];
	playerAll[3].x = rcvmsg[3];
	playerAll[4].x = rcvmsg[4];
	playerAll[5].x = rcvmsg[5];
	playerAll[6].x = rcvmsg[6];
	playerAll[7].x = rcvmsg[7];
	playerAll[8].x = rcvmsg[8];
	playerAll[9].x = rcvmsg[9];
	playerAll[0].y = rcvmsg[10];
	playerAll[1].y = rcvmsg[11];
	playerAll[2].y = rcvmsg[12];
	playerAll[3].y = rcvmsg[13];
	playerAll[4].y = rcvmsg[14];
	playerAll[5].y = rcvmsg[15];
	playerAll[6].y = rcvmsg[16];
	playerAll[7].y = rcvmsg[17];
	playerAll[8].y = rcvmsg[18];
	playerAll[9].y = rcvmsg[19];
	nike.x = rcvmsg[20];
	nike.y = rcvmsg[21];
	s.goalA = rcvmsg[22];
	s.goalB = rcvmsg[23];
	s.foulA = rcvmsg[24];
	s.foulB = rcvmsg[25];
	projectTimer.counter = rcvmsg[26];
}
void initializeXtft()
{
	int Status;
  	XTft_Config *TftConfigPtr;
  	unsigned int *col = 0x00ffffff;
  	TftConfigPtr = XTft_LookupConfig(XPAR_XPS_TFT_0_DEVICE_ID);
  	if (TftConfigPtr == (XTft_Config *)NULL) {
  		return XST_FAILURE;
  	}
  	Status = XTft_CfgInitialize(&TftInstance, TftConfigPtr,TftConfigPtr->BaseAddress);
  	if (Status != XST_SUCCESS) {
  		return XST_FAILURE;
  	}
  	vdm1 = TftConfigPtr->VideoMemBaseAddr;
  	initreadyScreen();
  	XTft_SetFrameBaseAddr(&TftInstance,vdm2);
  	XTft_SetColor(&TftInstance, 0, 0);
  	XTft_ClearScreen(&TftInstance);
  	drawField();
  	XTft_SetFrameBaseAddr(&TftInstance,vdm3);
  	XTft_SetColor(&TftInstance, 0, 0);
  	XTft_ClearScreen(&TftInstance);
  	drawField();
}
void initreadyScreen()
{
	int i,j;
	unsigned int *colg = 0x0000ff00;// green
	unsigned int *colr = 0x00ff0000;// red
	unsigned int *colbl = 0x000000ff;// blue
	unsigned int *colb = 0x00000000;// black
	unsigned int *coly = 0x00ffff00;// yellow
	for (i=20; i<620; i++)
	{
		XTft_SetPixel(&TftInstance, i, 20, colbl);
		XTft_SetPixel(&TftInstance, i, 400, colbl);
	}
	for (i=20; i<400; i++)
	{
		XTft_SetPixel(&TftInstance, 20, i, colbl);
		XTft_SetPixel(&TftInstance, 320, i, colbl);
		XTft_SetPixel(&TftInstance, 620, i, colbl);
	}
	XTft_SetPosChar(&TftInstance, 155,40);
	XTft_SetColor(&TftInstance, coly,colb);
	XTft_Write(&TftInstance,'T');
	XTft_Write(&TftInstance,'E');
	XTft_Write(&TftInstance,'A');
	XTft_Write(&TftInstance,'M');
	XTft_Write(&TftInstance,'-');
	XTft_Write(&TftInstance,'1');
	XTft_SetPosChar(&TftInstance, 455,40);
	XTft_SetColor(&TftInstance, coly,colb);
	XTft_Write(&TftInstance,'T');
	XTft_Write(&TftInstance,'E');
	XTft_Write(&TftInstance,'A');
	XTft_Write(&TftInstance,'M');
	XTft_Write(&TftInstance,'-');
	XTft_Write(&TftInstance,'2');
	for(i=1;i<=5;i++)
	{
	XTft_SetPosChar(&TftInstance, 150,50+i*30);
	XTft_SetColor(&TftInstance, colr,colb);
	XTft_Write(&TftInstance,'P');
	XTft_Write(&TftInstance,'L');
	XTft_Write(&TftInstance,'A');
	XTft_Write(&TftInstance,'Y');
	XTft_Write(&TftInstance,'E');
	XTft_Write(&TftInstance,'R');
	XTft_Write(&TftInstance,'-');
	switch (i) {
	case 1:
		XTft_Write(&TftInstance,'1');
	break;
	case 2:
		XTft_Write(&TftInstance,'2');
	break;
	case 3:
		XTft_Write(&TftInstance,'3');
	break;
	case 4:
		XTft_Write(&TftInstance,'4');
	break;
	case 5:
		XTft_Write(&TftInstance,'5');
	break;
	}
	XTft_SetPosChar(&TftInstance, 450,50+i*30);
	XTft_SetColor(&TftInstance, colr,colb);
	XTft_Write(&TftInstance,'P');
	XTft_Write(&TftInstance,'L');
	XTft_Write(&TftInstance,'A');
	XTft_Write(&TftInstance,'Y');
	XTft_Write(&TftInstance,'E');
	XTft_Write(&TftInstance,'R');
	XTft_Write(&TftInstance,'-');
	switch (i) {
	case 1:
		XTft_Write(&TftInstance,'1');
	break;
	case 2:
		XTft_Write(&TftInstance,'2');
	break;
	case 3:
		XTft_Write(&TftInstance,'3');
	break;
	case 4:
		XTft_Write(&TftInstance,'4');
	break;
	case 5:
		XTft_Write(&TftInstance,'5');
	break;
	}
	}
}
void checkplayerpacket()
{
	int i,team,player,done;
	done = 0;
	for(i=0;i<10;i++)
	{
		if(playerAll[i].x != 801 && playerAll[i].y != 801)
		{
			if(i<5)
			{
				team = 1;
				player = i;
				updatereadyScreen(team,player);
			}
			else
			{
				team = 2;
				player = i;
				updatereadyScreen(team,player);
			}
			done ++;
		}
	}
	if(done == 10)
		checkplayers = 0;
	else
		checkplayers = 1;
}
void updatereadyScreen(int team,int player)
{
	unsigned int *colg = 0x0000ff00;// green
	unsigned int *colr = 0x00ff0000;// red
	unsigned int *colbl = 0x000000ff;// blue
	unsigned int *colb = 0x00000000;// black
	unsigned int *coly = 0x00ffff00;// yellow
	int x,y;
	if(team == 1)
		{x = 150;
		y = 50+(player+1)*30;}
	else
		{x = 450;
		y = 50+(player-4)*30;}
	XTft_SetPosChar(&TftInstance,x,y);
	XTft_SetColor(&TftInstance, colg,colb);
	XTft_Write(&TftInstance,'P');
	XTft_Write(&TftInstance,'L');
	XTft_Write(&TftInstance,'A');
	XTft_Write(&TftInstance,'Y');
	XTft_Write(&TftInstance,'E');
	XTft_Write(&TftInstance,'R');
	XTft_Write(&TftInstance,'-');
	switch (player) {
	case 0:
		XTft_Write(&TftInstance,'1');
	break;
	case 1:
		XTft_Write(&TftInstance,'2');
	break;
	case 2:
		XTft_Write(&TftInstance,'3');
	break;
	case 3:
		XTft_Write(&TftInstance,'4');
	break;
	case 4:
		XTft_Write(&TftInstance,'5');
	break;
	case 5:
		XTft_Write(&TftInstance,'1');
	break;
	case 6:
		XTft_Write(&TftInstance,'2');
	break;
	case 7:
		XTft_Write(&TftInstance,'3');
	break;
	case 8:
		XTft_Write(&TftInstance,'4');
	break;
	case 9:
		XTft_Write(&TftInstance,'5');
	break;
	}
}
void drawRect(XTft *Tft, position pos, size s, unsigned int col)
{
	XTft_FillScreen(Tft, pos.x, pos.y, pos.x+s.width-1, pos.y+s.height-1, col);
}
void cliplotvdm(int xctr,int yctr,int x,int y,u32 vdm)
{
	XTft_MySetPixel(&TftInstance,xctr +x,yctr +y,0x00ffffff,vdm);
	XTft_MySetPixel(&TftInstance,xctr -x,yctr +y,0x00ffffff,vdm);
	XTft_MySetPixel(&TftInstance,xctr +x,yctr -y,0x00ffffff,vdm);
	XTft_MySetPixel(&TftInstance,xctr -x,yctr -y,0x00ffffff,vdm);
	XTft_MySetPixel(&TftInstance,xctr +y,yctr +x,0x00ffffff,vdm);
	XTft_MySetPixel(&TftInstance,xctr -y,yctr +x,0x00ffffff,vdm);
	XTft_MySetPixel(&TftInstance,xctr +y,yctr -x,0x00ffffff,vdm);
	XTft_MySetPixel(&TftInstance,xctr -y,yctr -x,0x00ffffff,vdm);
}
void drawCirclevdm(position pos, int r, unsigned int col,u32 vdm)
{
	 int x=0,y=r,p=1-r;
	 void cliplotvdm(int,int,int,int,u32);
	 cliplotvdm(pos.x,pos.y,x,y,vdm);

	 while(x<y)
	 {
		x++;
		if(p<0)
			p+=2*x+1;
		else
		{
			y--;
			p+=2*(x-y)+1;
		}
	  cliplotvdm(pos.x,pos.y,x,y,vdm);
	 }
}
void cliplot(int xctr,int yctr,int x,int y)
{
	XTft_SetPixel(&TftInstance,xctr +x,yctr +y,0x00ffffff);
	XTft_SetPixel(&TftInstance,xctr -x,yctr +y,0x00ffffff);
	XTft_SetPixel(&TftInstance,xctr +x,yctr -y,0x00ffffff);
	XTft_SetPixel(&TftInstance,xctr -x,yctr -y,0x00ffffff);
	XTft_SetPixel(&TftInstance,xctr +y,yctr +x,0x00ffffff);
	XTft_SetPixel(&TftInstance,xctr -y,yctr +x,0x00ffffff);
	XTft_SetPixel(&TftInstance,xctr +y,yctr -x,0x00ffffff);
	XTft_SetPixel(&TftInstance,xctr -y,yctr -x,0x00ffffff);
}
void drawCircle(position pos, int r, unsigned int col)
{
	 int x=0,y=r,p=1-r;
	 void cliplot(int,int,int,int);
	 cliplot(pos.x,pos.y,x,y);

	 while(x<y)
	 {
		x++;
		if(p<0)
			p+=2*x+1;
		else
		{
			y--;
			p+=2*(x-y)+1;
		}
	  cliplot(pos.x,pos.y,x,y);
	 }
}
void drawField()
{
		int i,j;
		unsigned int *colg = 0x0000ff00;// green
		unsigned int *colw = 0x00ffffff;// white
		unsigned int *colb = 0x00000000;// black
		unsigned int *coly = 0x00ffff00;// yellow
		unsigned int *colr = 0x00ff0000;// red
		unsigned int *colbl = 0x000000ff;// blue
		//Lawn
		position lawn_pos = {20,0};
		size lawn_size = {600,401};
		drawRect(&TftInstance, lawn_pos, lawn_size,colg);
		//Goal post
		position goal1_pos = {0,170};
		position goal2_pos = {620,170};
		size goal_size = {20,60};
		drawRect(&TftInstance, goal1_pos, goal_size,colw);
		drawRect(&TftInstance, goal2_pos, goal_size,colw);
		//Score Board
		for(i=410;i<470;i++)
		{
			XTft_SetPixel(&TftInstance, 220, i, colw);
			XTft_SetPixel(&TftInstance, 280, i, colw);
			XTft_SetPixel(&TftInstance, 360, i, colw);
			XTft_SetPixel(&TftInstance, 420, i, colw);
		}
		for(i=220;i<420;i++)
		{
			XTft_SetPixel(&TftInstance, i, 410, colw);
			XTft_SetPixel(&TftInstance, i, 470, colw);
		}
		for(i=220;i<280;i++)
		{
			XTft_SetPixel(&TftInstance, i, 430, colw);
			XTft_SetPixel(&TftInstance, i, 450, colw);
			XTft_SetPixel(&TftInstance, i+140, 430, colw);
			XTft_SetPixel(&TftInstance, i+140, 450, colw);
		}
			XTft_SetPosChar(&TftInstance, 222,415);
			XTft_SetColor(&TftInstance, colr,colb);
			XTft_Write(&TftInstance,'T');
			XTft_Write(&TftInstance,'E');
			XTft_Write(&TftInstance,'A');
			XTft_Write(&TftInstance,'M');
			XTft_Write(&TftInstance,'-');
			XTft_Write(&TftInstance,'1');
			XTft_SetPosChar(&TftInstance, 222,435);
			XTft_Write(&TftInstance,'S');
			XTft_Write(&TftInstance,'C');
			XTft_Write(&TftInstance,'O');
			XTft_Write(&TftInstance,'R');
			XTft_Write(&TftInstance,'E');
			XTft_Write(&TftInstance,'-');
			XTft_Write(&TftInstance,'0');
			XTft_SetPosChar(&TftInstance, 222,455);
			XTft_Write(&TftInstance,'F');
			XTft_Write(&TftInstance,'O');
			XTft_Write(&TftInstance,'U');
			XTft_Write(&TftInstance,'L');
			XTft_Write(&TftInstance,' ');
			XTft_Write(&TftInstance,'-');
			XTft_Write(&TftInstance,'0');
			XTft_SetPosChar(&TftInstance, 363,415);
			XTft_Write(&TftInstance,'T');
			XTft_Write(&TftInstance,'E');
			XTft_Write(&TftInstance,'A');
			XTft_Write(&TftInstance,'M');
			XTft_Write(&TftInstance,'-');
			XTft_Write(&TftInstance,'2');
			XTft_SetPosChar(&TftInstance, 363,435);
			XTft_Write(&TftInstance,'S');
			XTft_Write(&TftInstance,'C');
			XTft_Write(&TftInstance,'O');
			XTft_Write(&TftInstance,'R');
			XTft_Write(&TftInstance,'E');
			XTft_Write(&TftInstance,'-');
			XTft_Write(&TftInstance,'0');
			XTft_SetPosChar(&TftInstance, 363,455);
			XTft_Write(&TftInstance,'F');
			XTft_Write(&TftInstance,'O');
			XTft_Write(&TftInstance,'U');
			XTft_Write(&TftInstance,'L');
			XTft_Write(&TftInstance,' ');
			XTft_Write(&TftInstance,'-');
			XTft_Write(&TftInstance,'0');
		//Triangle Corners
		for (i=20; i<=40; i++){
			for (j=40-i; j>=0; j--){
				XTft_SetPixel(&TftInstance, i, j, colb);
			}
		}
		for (i=20; i<=40; i++){
			for (j=i; j<=40; j++){
				XTft_SetPixel(&TftInstance, i, j+360, colb);
			}
		}
		for (i=0; i<=20; i++){
			for (j=i; j>=0; j--){
				XTft_SetPixel(&TftInstance, i+600, j, colb);
			}
		}
		for (i=0; i<=20; i++){
			for (j=20-i; j<=20; j++){
				XTft_SetPixel(&TftInstance, i+600, j+380, colb);
			}
		}
		//Half line
		for (j=0; j<=400; j++)
		{
			XTft_SetPixel(&TftInstance, 320, j, colw);
		}
		//D Box
		for (j=160; j<=240; j++)
		{
			XTft_SetPixel(&TftInstance, 70, j, colw);
			XTft_SetPixel(&TftInstance, 570, j, colw);
		}
		for (i=20; i<70; i++)
		{
			XTft_SetPixel(&TftInstance, i, 160, colw);
			XTft_SetPixel(&TftInstance, i, 240, colw);
			XTft_SetPixel(&TftInstance, 550+i, 160, colw);
			XTft_SetPixel(&TftInstance, 550+i, 240, colw);
		}
		position cencircle_pos = {320,200};
		int rad = 50;
		drawCircle(cencircle_pos,rad,colw);
}
/*---------------------------------Ball UI Part ---------------------*/
/*void initbarray()
{
	u8 i,j,counter =0;
	for(i= -5;i<=5;i++)
	{
		for(j= -5;j<=5;j++)
		{
			if((i*i)+(j*j) < 25)
			{
				parray.pixel[0][counter]=i;
				parray.pixel[1][counter]=j;
				counter++;
			}
		}
	}
}*/

void initbarray()
{
		barray.pixel[0][0]=-4;
		barray.pixel[1][0]=-2;
		barray.pixel[0][1]=-4;
		barray.pixel[1][1]=-1;
		barray.pixel[0][2]=-4;
		barray.pixel[1][2]=0;
		barray.pixel[0][3]=-4;
		barray.pixel[1][3]=1;
		barray.pixel[0][4]=-4;
		barray.pixel[1][4]=2;
		barray.pixel[0][5]=-3;
		barray.pixel[1][5]=-3;
		barray.pixel[0][6]=-3;
		barray.pixel[1][6]=-2;
		barray.pixel[0][7]=-3;
		barray.pixel[1][7]=-1;
		barray.pixel[0][8]=-3;
		barray.pixel[1][8]=0;
		barray.pixel[0][9]=-3;
		barray.pixel[1][9]=1;
		barray.pixel[0][10]=-3;
		barray.pixel[1][10]=2;
		barray.pixel[0][11]=-3;
		barray.pixel[1][11]=3;
		barray.pixel[0][12]=-2;
		barray.pixel[1][12]=-4;
		barray.pixel[0][13]=-2;
		barray.pixel[1][13]=-3;
		barray.pixel[0][14]=-2;
		barray.pixel[1][14]=-2;
		barray.pixel[0][15]=-2;
		barray.pixel[1][15]=-1;
		barray.pixel[0][16]=-2;
		barray.pixel[1][16]=0;
		barray.pixel[0][17]=-2;
		barray.pixel[1][17]=1;
		barray.pixel[0][18]=-2;
		barray.pixel[1][18]=2;
		barray.pixel[0][19]=-2;
		barray.pixel[1][19]=3;
		barray.pixel[0][20]=-2;
		barray.pixel[1][20]=4;
		barray.pixel[0][21]=-1;
		barray.pixel[1][21]=-4;
		barray.pixel[0][22]=-1;
		barray.pixel[1][22]=-3;
		barray.pixel[0][23]=-1;
		barray.pixel[1][23]=-2;
		barray.pixel[0][24]=-1;
		barray.pixel[1][24]=-1;
		barray.pixel[0][25]=-1;
		barray.pixel[1][25]=0;
		barray.pixel[0][26]=-1;
		barray.pixel[1][26]=1;
		barray.pixel[0][27]=-1;
		barray.pixel[1][27]=2;
		barray.pixel[0][28]=-1;
		barray.pixel[1][28]=3;
		barray.pixel[0][29]=-1;
		barray.pixel[1][29]=4;
		barray.pixel[0][30]=0;
		barray.pixel[1][30]=-4;
		barray.pixel[0][31]=0;
		barray.pixel[1][31]=-3;
		barray.pixel[0][32]=0;
		barray.pixel[1][32]=-2;
		barray.pixel[0][33]=0;
		barray.pixel[1][33]=-1;
		barray.pixel[0][34]=0;
		barray.pixel[1][34]=0;
		barray.pixel[0][35]=0;
		barray.pixel[1][35]=1;
		barray.pixel[0][36]=0;
		barray.pixel[1][36]=2;
		barray.pixel[0][37]=0;
		barray.pixel[1][37]=3;
		barray.pixel[0][38]=0;
		barray.pixel[1][38]=4;
		barray.pixel[0][39]=1;
		barray.pixel[1][39]=-4;
		barray.pixel[0][40]=1;
		barray.pixel[1][40]=-3;
		barray.pixel[0][41]=1;
		barray.pixel[1][41]=-2;
		barray.pixel[0][42]=1;
		barray.pixel[1][42]=-1;
		barray.pixel[0][43]=1;
		barray.pixel[1][43]=0;
		barray.pixel[0][44]=1;
		barray.pixel[1][44]=1;
		barray.pixel[0][45]=1;
		barray.pixel[1][45]=2;
		barray.pixel[0][46]=1;
		barray.pixel[1][46]=3;
		barray.pixel[0][47]=1;
		barray.pixel[1][47]=4;
		barray.pixel[0][48]=2;
		barray.pixel[1][48]=-4;
		barray.pixel[0][49]=2;
		barray.pixel[1][49]=-3;
		barray.pixel[0][50]=2;
		barray.pixel[1][50]=-2;
		barray.pixel[0][51]=2;
		barray.pixel[1][51]=-1;
		barray.pixel[0][52]=2;
		barray.pixel[1][52]=0;
		barray.pixel[0][53]=2;
		barray.pixel[1][53]=1;
		barray.pixel[0][54]=2;
		barray.pixel[1][54]=2;
		barray.pixel[0][55]=2;
		barray.pixel[1][55]=3;
		barray.pixel[0][56]=2;
		barray.pixel[1][56]=4;
		barray.pixel[0][57]=3;
		barray.pixel[1][57]=-3;
		barray.pixel[0][58]=3;
		barray.pixel[1][58]=-2;
		barray.pixel[0][59]=3;
		barray.pixel[1][59]=-1;
		barray.pixel[0][60]=3;
		barray.pixel[1][60]=0;
		barray.pixel[0][61]=3;
		barray.pixel[1][61]=1;
		barray.pixel[0][62]=3;
		barray.pixel[1][62]=2;
		barray.pixel[0][63]=3;
		barray.pixel[1][63]=3;
		barray.pixel[0][64]=4;
		barray.pixel[1][64]=-2;
		barray.pixel[0][65]=4;
		barray.pixel[1][65]=-1;
		barray.pixel[0][66]=4;
		barray.pixel[1][66]=0;
		barray.pixel[0][67]=4;
		barray.pixel[1][67]=1;
		barray.pixel[0][68]=4;
		barray.pixel[1][68]=2;
}

void drawBall(u32 vdm)
{
	int i;
	unsigned int *colb = 0x00000000;// black
	for(i=0;i<69;i++)
	{
		XTft_MySetPixel(&TftInstance,nike.x+barray.pixel[0][i],nike.y+barray.pixel[1][i],colb,vdm);
	}
}
void initBall(u32 vdm)
{
	initbarray();
	if(vdm == vdm2 && pass1 == 0)
	{
		prevnike_2.x = nike.x;
		prevnike_2.y = nike.y;
	}
	else
	{
		if(vdm == vdm3 && pass2 == 0)
		{
			prevnike_3.x = nike.x;
			prevnike_3.y = nike.y;
		}
		else
		{}
	}
}
void setballcolormap(u32 vdm)
{
	int i;
	unsigned int *colg = 0x0000ff00;// green
	for(i=0;i<69;i++)
	{
		if(vdm == vdm2)
		{
			XTft_MySetPixel(&TftInstance,prevnike_2.x+barray.pixel[0][i],prevnike_2.y+barray.pixel[1][i],colg,vdm);
		}
		else
		{
			XTft_MySetPixel(&TftInstance,prevnike_3.x+barray.pixel[0][i],prevnike_3.y+barray.pixel[1][i],colg,vdm);
		}
	}
}
void updateBall_1(u32 vdm)
{
	setballcolormap(vdm);
}
void updateBall_2(u32 vdm)
{
	drawBall(vdm);
	if(vdm == vdm2)
	{
		prevnike_2.x = nike.x;
		prevnike_2.y = nike.y;
	}
	else
	{
		prevnike_3.x = nike.x;
		prevnike_3.y = nike.y;
	}
}
/*---------------------------------Player UI Part ---------------------*/
/*void initparray()
{
	u8 i,j,counter =0;
	for(i= -7;i<=7;i++)
	{
		for(j= -7;j<=7;j++)
		{
			if((i*i)+(j*j) < 49)
			{
				parray.pixel[0][counter]=i;
				parray.pixel[1][counter]=j;
				counter++;
			}
		}
	}
}*/

void initparray()
{
		parray.pixel[0][0]=-6;
		parray.pixel[1][0]=-3;
		parray.pixel[0][1]=-6;
		parray.pixel[1][1]=-2;
		parray.pixel[0][2]=-6;
		parray.pixel[1][2]=-1;
		parray.pixel[0][3]=-6;
		parray.pixel[1][3]=0;
		parray.pixel[0][4]=-6;
		parray.pixel[1][4]=1;
		parray.pixel[0][5]=-6;
		parray.pixel[1][5]=2;
		parray.pixel[0][6]=-6;
		parray.pixel[1][6]=3;
		parray.pixel[0][7]=-5;
		parray.pixel[1][7]=-4;
		parray.pixel[0][8]=-5;
		parray.pixel[1][8]=-3;
		parray.pixel[0][9]=-5;
		parray.pixel[1][9]=-2;
		parray.pixel[0][10]=-5;
		parray.pixel[1][10]=-1;
		parray.pixel[0][11]=-5;
		parray.pixel[1][11]=0;
		parray.pixel[0][12]=-5;
		parray.pixel[1][12]=1;
		parray.pixel[0][13]=-5;
		parray.pixel[1][13]=2;
		parray.pixel[0][14]=-5;
		parray.pixel[1][14]=3;
		parray.pixel[0][15]=-5;
		parray.pixel[1][15]=4;
		parray.pixel[0][16]=-4;
		parray.pixel[1][16]=-5;
		parray.pixel[0][17]=-4;
		parray.pixel[1][17]=-4;
		parray.pixel[0][18]=-4;
		parray.pixel[1][18]=-3;
		parray.pixel[0][19]=-4;
		parray.pixel[1][19]=-2;
		parray.pixel[0][20]=-4;
		parray.pixel[1][20]=-1;
		parray.pixel[0][21]=-4;
		parray.pixel[1][21]=0;
		parray.pixel[0][22]=-4;
		parray.pixel[1][22]=1;
		parray.pixel[0][23]=-4;
		parray.pixel[1][23]=2;
		parray.pixel[0][24]=-4;
		parray.pixel[1][24]=3;
		parray.pixel[0][25]=-4;
		parray.pixel[1][25]=4;
		parray.pixel[0][26]=-4;
		parray.pixel[1][26]=5;
		parray.pixel[0][27]=-3;
		parray.pixel[1][27]=-6;
		parray.pixel[0][28]=-3;
		parray.pixel[1][28]=-5;
		parray.pixel[0][29]=-3;
		parray.pixel[1][29]=-4;
		parray.pixel[0][30]=-3;
		parray.pixel[1][30]=-3;
		parray.pixel[0][31]=-3;
		parray.pixel[1][31]=-2;
		parray.pixel[0][32]=-3;
		parray.pixel[1][32]=-1;
		parray.pixel[0][33]=-3;
		parray.pixel[1][33]=0;
		parray.pixel[0][34]=-3;
		parray.pixel[1][34]=1;
		parray.pixel[0][35]=-3;
		parray.pixel[1][35]=2;
		parray.pixel[0][36]=-3;
		parray.pixel[1][36]=3;
		parray.pixel[0][37]=-3;
		parray.pixel[1][37]=4;
		parray.pixel[0][38]=-3;
		parray.pixel[1][38]=5;
		parray.pixel[0][39]=-3;
		parray.pixel[1][39]=6;
		parray.pixel[0][40]=-2;
		parray.pixel[1][40]=-6;
		parray.pixel[0][41]=-2;
		parray.pixel[1][41]=-5;
		parray.pixel[0][42]=-2;
		parray.pixel[1][42]=-4;
		parray.pixel[0][43]=-2;
		parray.pixel[1][43]=-3;
		parray.pixel[0][44]=-2;
		parray.pixel[1][44]=-2;
		parray.pixel[0][45]=-2;
		parray.pixel[1][45]=-1;
		parray.pixel[0][46]=-2;
		parray.pixel[1][46]=0;
		parray.pixel[0][47]=-2;
		parray.pixel[1][47]=1;
		parray.pixel[0][48]=-2;
		parray.pixel[1][48]=2;
		parray.pixel[0][49]=-2;
		parray.pixel[1][49]=3;
		parray.pixel[0][50]=-2;
		parray.pixel[1][50]=4;
		parray.pixel[0][51]=-2;
		parray.pixel[1][51]=5;
		parray.pixel[0][52]=-2;
		parray.pixel[1][52]=6;
		parray.pixel[0][53]=-1;
		parray.pixel[1][53]=-6;
		parray.pixel[0][54]=-1;
		parray.pixel[1][54]=-5;
		parray.pixel[0][55]=-1;
		parray.pixel[1][55]=-4;
		parray.pixel[0][56]=-1;
		parray.pixel[1][56]=-3;
		parray.pixel[0][57]=-1;
		parray.pixel[1][57]=-2;
		parray.pixel[0][58]=-1;
		parray.pixel[1][58]=-1;
		parray.pixel[0][59]=-1;
		parray.pixel[1][59]=0;
		parray.pixel[0][60]=-1;
		parray.pixel[1][60]=1;
		parray.pixel[0][61]=-1;
		parray.pixel[1][61]=2;
		parray.pixel[0][62]=-1;
		parray.pixel[1][62]=3;
		parray.pixel[0][63]=-1;
		parray.pixel[1][63]=4;
		parray.pixel[0][64]=-1;
		parray.pixel[1][64]=5;
		parray.pixel[0][65]=-1;
		parray.pixel[1][65]=6;
		parray.pixel[0][66]=0;
		parray.pixel[1][66]=-6;
		parray.pixel[0][67]=0;
		parray.pixel[1][67]=-5;
		parray.pixel[0][68]=0;
		parray.pixel[1][68]=-4;
		parray.pixel[0][69]=0;
		parray.pixel[1][69]=-3;
		parray.pixel[0][70]=0;
		parray.pixel[1][70]=-2;
		parray.pixel[0][71]=0;
		parray.pixel[1][71]=-1;
		parray.pixel[0][72]=0;
		parray.pixel[1][72]=0;
		parray.pixel[0][73]=0;
		parray.pixel[1][73]=1;
		parray.pixel[0][74]=0;
		parray.pixel[1][74]=2;
		parray.pixel[0][75]=0;
		parray.pixel[1][75]=3;
		parray.pixel[0][76]=0;
		parray.pixel[1][76]=4;
		parray.pixel[0][77]=0;
		parray.pixel[1][77]=5;
		parray.pixel[0][78]=0;
		parray.pixel[1][78]=6;
		parray.pixel[0][79]=1;
		parray.pixel[1][79]=-6;
		parray.pixel[0][80]=1;
		parray.pixel[1][80]=-5;
		parray.pixel[0][81]=1;
		parray.pixel[1][81]=-4;
		parray.pixel[0][82]=1;
		parray.pixel[1][82]=-3;
		parray.pixel[0][83]=1;
		parray.pixel[1][83]=-2;
		parray.pixel[0][84]=1;
		parray.pixel[1][84]=-1;
		parray.pixel[0][85]=1;
		parray.pixel[1][85]=0;
		parray.pixel[0][86]=1;
		parray.pixel[1][86]=1;
		parray.pixel[0][87]=1;
		parray.pixel[1][87]=2;
		parray.pixel[0][88]=1;
		parray.pixel[1][88]=3;
		parray.pixel[0][89]=1;
		parray.pixel[1][89]=4;
		parray.pixel[0][90]=1;
		parray.pixel[1][90]=5;
		parray.pixel[0][91]=1;
		parray.pixel[1][91]=6;
		parray.pixel[0][92]=2;
		parray.pixel[1][92]=-6;
		parray.pixel[0][93]=2;
		parray.pixel[1][93]=-5;
		parray.pixel[0][94]=2;
		parray.pixel[1][94]=-4;
		parray.pixel[0][95]=2;
		parray.pixel[1][95]=-3;
		parray.pixel[0][96]=2;
		parray.pixel[1][96]=-2;
		parray.pixel[0][97]=2;
		parray.pixel[1][97]=-1;
		parray.pixel[0][98]=2;
		parray.pixel[1][98]=0;
		parray.pixel[0][99]=2;
		parray.pixel[1][99]=1;
		parray.pixel[0][100]=2;
		parray.pixel[1][100]=2;
		parray.pixel[0][101]=2;
		parray.pixel[1][101]=3;
		parray.pixel[0][102]=2;
		parray.pixel[1][102]=4;
		parray.pixel[0][103]=2;
		parray.pixel[1][103]=5;
		parray.pixel[0][104]=2;
		parray.pixel[1][104]=6;
		parray.pixel[0][105]=3;
		parray.pixel[1][105]=-6;
		parray.pixel[0][106]=3;
		parray.pixel[1][106]=-5;
		parray.pixel[0][107]=3;
		parray.pixel[1][107]=-4;
		parray.pixel[0][108]=3;
		parray.pixel[1][108]=-3;
		parray.pixel[0][109]=3;
		parray.pixel[1][109]=-2;
		parray.pixel[0][110]=3;
		parray.pixel[1][110]=-1;
		parray.pixel[0][111]=3;
		parray.pixel[1][111]=0;
		parray.pixel[0][112]=3;
		parray.pixel[1][112]=1;
		parray.pixel[0][113]=3;
		parray.pixel[1][113]=2;
		parray.pixel[0][114]=3;
		parray.pixel[1][114]=3;
		parray.pixel[0][115]=3;
		parray.pixel[1][115]=4;
		parray.pixel[0][116]=3;
		parray.pixel[1][116]=5;
		parray.pixel[0][117]=3;
		parray.pixel[1][117]=6;
		parray.pixel[0][118]=4;
		parray.pixel[1][118]=-5;
		parray.pixel[0][119]=4;
		parray.pixel[1][119]=-4;
		parray.pixel[0][120]=4;
		parray.pixel[1][120]=-3;
		parray.pixel[0][121]=4;
		parray.pixel[1][121]=-2;
		parray.pixel[0][122]=4;
		parray.pixel[1][122]=-1;
		parray.pixel[0][123]=4;
		parray.pixel[1][123]=0;
		parray.pixel[0][124]=4;
		parray.pixel[1][124]=1;
		parray.pixel[0][125]=4;
		parray.pixel[1][125]=2;
		parray.pixel[0][126]=4;
		parray.pixel[1][126]=3;
		parray.pixel[0][127]=4;
		parray.pixel[1][127]=4;
		parray.pixel[0][128]=4;
		parray.pixel[1][128]=5;
		parray.pixel[0][129]=5;
		parray.pixel[1][129]=-4;
		parray.pixel[0][130]=5;
		parray.pixel[1][130]=-3;
		parray.pixel[0][131]=5;
		parray.pixel[1][131]=-2;
		parray.pixel[0][132]=5;
		parray.pixel[1][132]=-1;
		parray.pixel[0][133]=5;
		parray.pixel[1][133]=0;
		parray.pixel[0][134]=5;
		parray.pixel[1][134]=1;
		parray.pixel[0][135]=5;
		parray.pixel[1][135]=2;
		parray.pixel[0][136]=5;
		parray.pixel[1][136]=3;
		parray.pixel[0][137]=5;
		parray.pixel[1][137]=4;
		parray.pixel[0][138]=6;
		parray.pixel[1][138]=-3;
		parray.pixel[0][139]=6;
		parray.pixel[1][139]=-2;
		parray.pixel[0][140]=6;
		parray.pixel[1][140]=-1;
		parray.pixel[0][141]=6;
		parray.pixel[1][141]=0;
		parray.pixel[0][142]=6;
		parray.pixel[1][142]=1;
		parray.pixel[0][143]=6;
		parray.pixel[1][143]=2;
		parray.pixel[0][144]=6;
		parray.pixel[1][144]=3;
}
void drawPlayer(u32 vdm)
{
	int i,j;
	unsigned int *colr = 0x00ff0000;// red
	unsigned int *colbl = 0x000000ff;// blue
	for(j=0;j<145;j++)
	{
		for(i=0;i<5;i++)
			XTft_MySetPixel(&TftInstance,playerAll[i].x+parray.pixel[0][j],playerAll[i].y+parray.pixel[1][j],colr,vdm);
		for(i=5;i<10;i++)
			XTft_MySetPixel(&TftInstance,playerAll[i].x+parray.pixel[0][j],playerAll[i].y+parray.pixel[1][j],colbl,vdm);
	}
}
void initPlayer(u32 vdm)
{
	int i;
	initparray();
	if(vdm == vdm2 && pass1 == 0)
		{
			for(i=0;i<10;i++)
			{
				prevplayerAll_2[i].x = playerAll[i].x;
				prevplayerAll_2[i].y = playerAll[i].y;
			}
		}
		else
		{
			if(vdm == vdm3 && pass2 == 0)
			{
				for(i=0;i<10;i++)
				{
					prevplayerAll_3[i].x = playerAll[i].x;
					prevplayerAll_3[i].y = playerAll[i].y;
				}
			}
			else
			{}
		}
}
void setplayercolormap(u32 vdm)
{
	int i,j;
	unsigned int *colg = 0x0000ff00;// green
	for(i=0;i<10;i++)
	{
		for(j=0;j<145;j++)
		{
			if(vdm == vdm2)
			{
				XTft_MySetPixel(&TftInstance,prevplayerAll_2[i].x+parray.pixel[0][j],prevplayerAll_2[i].y+parray.pixel[1][j],colg,vdm);
			}
			else
			{
				XTft_MySetPixel(&TftInstance,prevplayerAll_3[i].x+parray.pixel[0][j],prevplayerAll_3[i].y+parray.pixel[1][j],colg,vdm);
			}
		}
	}
}
void updatePlayer_1(u32 vdm)
{
	int i;
	setplayercolormap(vdm);
}
void updatePlayer_2(u32 vdm)
{
	int i;
	drawPlayer(vdm);
	if(vdm == vdm2)
	{
		for( i=0;i<10;i++)
			{
				prevplayerAll_2[i].x = playerAll[i].x;
				prevplayerAll_2[i].y = playerAll[i].y;
			}
	}
	else
	{
		for( i=0;i<10;i++)
			{
				prevplayerAll_3[i].x = playerAll[i].x;
				prevplayerAll_3[i].y = playerAll[i].y;
			}
	}
}
void drawClock()
{
	unsigned int *coly = 0x00ffff00;// yellow
	unsigned int *colb = 0x00000000;// black
	projectTimer.minutes = projectTimer.counter/60;
	projectTimer.seconds = projectTimer.counter%60;
	projectTimer.min = projectTimer.minutes + 48;
	projectTimer.sec_1 = (projectTimer.seconds/10) + 48;
	projectTimer.sec_2 = (projectTimer.seconds%10) + 48;
	XTft_SetPosChar(&TftInstance, 300,440);
	XTft_SetColor(&TftInstance, coly,colb);
	XTft_Write(&TftInstance,'0');
	XTft_Write(&TftInstance,projectTimer.min);
	XTft_Write(&TftInstance,':');
	XTft_Write(&TftInstance,projectTimer.sec_1);
	XTft_Write(&TftInstance,projectTimer.sec_2);
}
void drawContents(u32 vdm)
{
	unsigned int *colw = 0x00ffffff;// white
	int i,j;
	//Goal post
	position goal1_pos = {0,170};
	position goal2_pos = {620,170};
	size goal_size = {20,60};
	drawRect(&TftInstance, goal1_pos, goal_size,colw);
	drawRect(&TftInstance, goal2_pos, goal_size,colw);
	//Half line
	for (j=0; j<=400; j++)
	{
		XTft_MySetPixel(&TftInstance, 320, j, colw,vdm);
	}
	//D Box
	for (j=160; j<=240; j++)
	{
		XTft_MySetPixel(&TftInstance, 70, j, colw,vdm);
		XTft_MySetPixel(&TftInstance, 570, j, colw,vdm);
	}
	for (i=20; i<70; i++)
	{
		XTft_MySetPixel(&TftInstance, i, 160, colw,vdm);
		XTft_MySetPixel(&TftInstance, i, 240, colw,vdm);
		XTft_MySetPixel(&TftInstance, 550+i, 160, colw,vdm);
		XTft_MySetPixel(&TftInstance, 550+i, 240, colw,vdm);
	}
	position cencircle_pos = {320,200};
	int rad = 50;
	drawCirclevdm(cencircle_pos,rad,colw,vdm);
}
void drawScoreboard()
{
	unsigned int *colr = 0x00ff0000;// red
	unsigned int *colb = 0x00000000;// black
	char s_1,s_2,f_1,f_2;
	s_1 = s.goalA+48;
	s_2= s.goalB+48;
	f_1= s.foulA+48;
	f_2= s.foulB+48;
	XTft_SetPosChar(&TftInstance, 270,435);
	XTft_SetColor(&TftInstance, colr,colb);
	XTft_Write(&TftInstance,s_1);
	XTft_SetPosChar(&TftInstance, 270,455);
	XTft_Write(&TftInstance,f_1);
	XTft_SetPosChar(&TftInstance, 411,435);
	XTft_Write(&TftInstance,s_2);
	XTft_SetPosChar(&TftInstance, 411,455);
	XTft_Write(&TftInstance,f_2);
}
void gamestart(u32 vdm)
{
	updateBall_1(vdm);
	updatePlayer_1(vdm);
	drawContents(vdm);
	updateBall_2(vdm);
	updatePlayer_2(vdm);
	drawScoreboard();
	drawClock();
}

int main (int argc, const char * argv[]) {
	xilkernel_main();
	return 0;
}

void main_prog ()
{
	u8 flag = 0;
	u32 vdm2pass;
	// This thread is statically created (as configured in the kernel configuration) and has priority 0 (This is the highest possible)
	initializeMailbox();
	initializeXtft();
	while(checkplayers == 1)
	{
		XTft_SetFrameBaseAddr(&TftInstance,vdm1);
		recv ();
		checkplayerpacket();
	}
	while(1){
		if(flag == 0)
		{
			vdm2pass = vdm2;
			flag = 1;
			if(pass1 == 0)
			{
				XTft_SetFrameBaseAddr(&TftInstance,vdm2);
				initBall(vdm2);
				initPlayer(vdm2);
				pass1 = 1;
			}
			else
			{
				recv ();
			}
			gamestart(vdm2pass);
			while (XTft_GetVsyncStatus(&TftInstance) != XTFT_IESR_VADDRLATCH_STATUS_MASK);
				XTft_SetFrameBaseAddr(&TftInstance, vdm2pass);
		}
		else
		{
			vdm2pass = vdm3;
			flag = 0;
			if(pass2 == 0)
			{
				recv ();
				XTft_SetFrameBaseAddr(&TftInstance,vdm3);
				initBall(vdm3);
				initPlayer(vdm3);
				pass2 = 1;
			}
			else
			{
				recv ();
			}
			gamestart(vdm2pass);
			while (XTft_GetVsyncStatus(&TftInstance) != XTFT_IESR_VADDRLATCH_STATUS_MASK);
				XTft_SetFrameBaseAddr(&TftInstance, vdm2pass);
		}
	}
}
