#define Touch_Control (*(volatile unsigned char *)(0x84000230))
#define Touch_Status (*(volatile unsigned char *)(0x84000230))
#define Touch_TxData (*(volatile unsigned char *)(0x84000232))
#define Touch_RxData (*(volatile unsigned char *)(0x84000232))
#define Touch_Baud 	(*(volatile unsigned char *)(0x84000234))

#include <stdio.h>

/* a data type to hold a point/coord */
typedef struct { int x, y; } Point ;
void Init_Touch(void);
int ScreenTouched( void );
void WaitForTouch();
Point GetPress(void);
Point GetRelease(void);
int putcharTouch(int c);
int getcharTouch(void);


//int main()
//{
//  printf("Hello from Nios II!\n");
//
//  int i = 0;
//  Init_Touch();
//  printf("Finished init\n");
//
//  Point p;
//  while(1){
//	 // printf("Screen Touched = %d\n", ScreenTouched());
//	  p = GetPress();
//	  printf("POINT PRESSED = (%d, %d)\n", p.x, p.y);
//	  p = GetRelease();
//	  printf("POINT RELEASED = (%d, %d)\n", p.x, p.y);
//	  printf("---------------\n");
//  }
//  return 0;
//}



/*****************************************************************************
** Initialise touch screen controller
*****************************************************************************/
void Init_Touch(void)
{
 // set up 6850 Control Register to utilise a divide by 16 clock,
 // set RTS low, use 8 bits of data, no parity, 1 stop bit,
 // transmitter interrupt disabled
	Touch_Control = 0b00010101;

 // Program 6850 and baud rate generator to communicate with Touch
	Touch_Baud = 0xff; //9600

 // send Touch controller an "enable touch" command
	putcharTouch(0x55);
	putcharTouch(0x01);
	putcharTouch(0x12);


}

/*****************************************************************************
** test if screen touched
*****************************************************************************/
int ScreenTouched( void )
{
 // return TRUE if any data received from 6850 connected to Touch
 // or FALSE otherwise
	return (Touch_Status & 2 == 0);
}

/*****************************************************************************
** wait for screen to be touched
*****************************************************************************/
void WaitForTouch()
{
	while(!ScreenTouched()){

	}
}

/*****************************************************************************
* This function waits for a touch screen press event and returns X,Y coord
*****************************************************************************/
Point GetPress(void)
{
 Point p1;
 // wait for a pen down command then return the X,Y coord of the point
	 int byte1 = getcharTouch();
	 while(getcharTouch() != 0x81){
  	 //printf("waiting for pen down command\n");
   }

 	 int byte2 = getcharTouch();
 	 int byte3 = getcharTouch();
 	 int byte4 = getcharTouch();
 	 int byte5 = getcharTouch();

 	 printf("1 byte = %#010x\n", byte1);
	 printf("2 byte = %#010x\n", byte2);
	 printf("3 byte = %#010x\n", byte3);
	 printf("4 byte = %#010x\n", byte4);
	 printf("5 byte = %#010x\n", byte5);

  	 p1.x = (byte2 >> 1) + (byte3 << 7);
 	 p1.y = ((byte4 >> 1) + (byte5 << 7));


  // calibrated correctly so that it maps to a pixel on screen
  return p1;
}

/*****************************************************************************
* This function waits for a touch screen release event and returns X,Y coord
*****************************************************************************/
Point GetRelease(void)
{
 Point p1;
 // wait for a pen up command then return the X,Y coord of the point
 int byte1 = getcharTouch();
 while(getcharTouch() != 0x80){
 	 //printf("waiting for pen up command\n");
  }

	 int byte2 = getcharTouch();
	 int byte3 = getcharTouch();
	 int byte4 = getcharTouch();
	 int byte5 = getcharTouch();

 	 printf("1 byte = %#010x\n", byte1);
 	 printf("2 byte = %#010x\n", byte2);
 	 printf("3 byte = %#010x\n", byte3);
 	 printf("4 byte = %#010x\n", byte4);
 	 printf("5 byte = %#010x\n", byte5);

 	 p1.x = (byte2 >> 1) + (byte3 << 7);
	 p1.y = ((byte4 >> 1) + (byte5 << 7));

	 return p1;
}

int putcharTouch(int c)
{
 // poll Tx bit(bit 0) in 6850 status register. Wait for it to become '1'
 while((Touch_Status >> 1) % 2 == 0);
 Touch_TxData = c;
 return c; // return c
}

int getcharTouch( void )
{
 // poll Rx bit in 6850 status register. Wait for it to become '1'
 while(Touch_Status % 2 == 0);
 return Touch_RxData;
}
