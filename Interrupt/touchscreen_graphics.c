#include <stdio.h>
#include <string.h>
#include "sys/alt_flash.h"

#include "sys/alt_irq.h"
#include "system.h"

#include "altera_avalon_pio_regs.h"
#include "altera_avalon_timer_regs.h"

#include "touchscreen.c"

//Base address from Qsys
#define TIMER_BASE 0x2080
#define TIMER_IRQ_INTERRUPT_CONTROLLER_ID 4
#define TIMER_IRQ 4
#define LED_PWM_BASE 0x2010
#define LED_PIO_BASE 0x2010
#define HEX0_1_PIO_BASE (volatile char *) 0x2030

//********************** Global Variables ***********************************************
volatile unsigned short int led_dir;
volatile Point pointPressed;
volatile Point pointReleased;
volatile unsigned char i = 0;

//********************** ISR functions (Touchscreen)**************************************************
static void timer_isr (void * context)
{ volatile unsigned char* led_ptr;
  volatile unsigned char* dir_ptr;

  IOWR_ALTERA_AVALON_TIMER_STATUS(TIMER_BASE, 0);

  dir_ptr = (volatile unsigned char*)context;
  led_ptr = dir_ptr + 1;

  IOWR_ALTERA_AVALON_PIO_DATA(LED_PIO_BASE, *led_ptr);

  //turns on leds in forwards and backwards directions just to check interrupt works
  if(*dir_ptr)
    { *led_ptr >>= 1;
      i--;
    }
  else
    { *led_ptr <<= 1;
      i++;
    }
  if(*led_ptr & 0x81)
    *dir_ptr ^= 0x1;

  //updates touchscreen points
  pointPressed = GetPress();
  pointReleased = GetRelease();

}

//********************** Graphics functions ********************************************
void draw(void)
{
    *HEX0_1_PIO_BASE = 0x03;	//displays 3 on hex 0
    printf("Pressed at (%d, %d)\n", pointPressed.x, pointPressed.y);
    printf("Released at (%d, %d)\n", pointReleased.x, pointReleased.y);

    //Graphics code...
}

//********************* Main function ***************************************************
int main (void)
{ void* led_dir_ptr = (void*)&led_dir;

  led_dir = 0x0100;

  //Touchscreen Initialization
  Init_Touch();

  //Timer Initialization
  IOWR_ALTERA_AVALON_TIMER_CONTROL(TIMER_BASE, 0x0003);
  IOWR_ALTERA_AVALON_TIMER_STATUS(TIMER_BASE, 0);

  //This can be changed to alter timer freq
  IOWR_ALTERA_AVALON_TIMER_PERIODL(TIMER_BASE, 0x9f00);
  IOWR_ALTERA_AVALON_TIMER_PERIODH(TIMER_BASE, 0x0024);

  //Register ISR for timer event
  alt_ic_isr_register(TIMER_IRQ_INTERRUPT_CONTROLLER_ID, TIMER_IRQ, timer_isr, led_dir_ptr, 0);


  //Start timer and begin the work
  IOWR_ALTERA_AVALON_TIMER_CONTROL(TIMER_BASE, 0x0007);


  while (1){
	  draw();
  }
}
