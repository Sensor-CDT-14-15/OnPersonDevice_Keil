#include <MKL25Z4.H>
#include "uart.h"
#include "gps.h"
#include "MKL25z_I2C.h"
#include "spi.h"
#include <vector>
#include <cmath>
#include <iostream>
#include "MMA8451Q.h"
#include <iterator>

using namespace std;

#define LED_NUM     3                   /* Number of user LEDs                */
volatile uint32_t msTicks;                            /* counts 1ms timeTicks */
char gpsbuffer[256];										//Buffer for GPS string
int lock;
float longitude;
float latitude;
int count;
int oriantation;

/*----------------------------------------------------------------------------
  SysTick_Handler
 *----------------------------------------------------------------------------*/
void SysTick_Handler(void) {
  msTicks++;                        /* increment counter necessary in Delay() */
}

/*------------------------------------------------------------------------------
  delays number of tick Systicks (happens every 1 ms)
 *------------------------------------------------------------------------------*/
__INLINE static void Delay (uint32_t dlyTicks) {
  uint32_t curTicks;

  curTicks = msTicks;
  while ((msTicks - curTicks) < dlyTicks);
}

/*------------------------------------------------------------------------------
  configer LED pins
 *------------------------------------------------------------------------------*/
__INLINE static void LED_Config(void) {

  SIM->SCGC5    |= (1UL <<  10) | (1UL <<  12);      /* Enable Clock to Port B & D */ 
  PORTB->PCR[18] = (1UL <<  8);                      /* Pin PTB18 is GPIO */
  PORTB->PCR[19] = (1UL <<  8);                      /* Pin PTB19 is GPIO */
  PORTD->PCR[1]  = (1UL <<  8);                      /* Pin PTD1  is GPIO */

	FPTB->PDOR |=  (1UL << 18);							// enable PTB18 as an output 
	FPTB->PDDR |= (1ul << 19);							//enable PTB19 as an output
}

/*------------------------------------------------------------------------------
  Switch on LEDs/pins
 *------------------------------------------------------------------------------*/
__INLINE static void LED_On (void) {
	
	FPTB->PDOR	|= (1UL << 18);													//Set Port B pin 18 high
	FPTB->PDOR	|= (1UL << 19);													//Set Port B pin 19 high
}

/*------------------------------------------------------------------------------
  Switch off LEDs
 *------------------------------------------------------------------------------*/
__INLINE static void LED_Off (void) {
	
	FPTB->PDOR	&= ~(1UL << 18);												//Set Port B pin 18 Low
	FPTB->PDOR	&= ~(1UL << 19);												//Set Port B pin 18 High
	
}
/*----------------------------------------------------------------------------
  MAIN function
 *----------------------------------------------------------------------------*/

void get_serial(void)
{
	int i;
	int j; 
			char buffer[9];
		for(i=0;i<3;i++)
		{
				while(!uart0_getchar_present());
				buffer[i] = uart0_getchar();
				uart0_putchar('G');
		}
}

void GPSline(void) {
	int i; 	
	count += 1;
	while(!uart0_getchar_present());
	 while(uart0_getchar() != '$');// wait for the start of a line
		for(i=0;i<256;i++)
		{
				while(!uart0_getchar_present());
				gpsbuffer[i] = uart0_getchar();
//				uart0_putchar(gpsbuffer[i]);
			
			if(gpsbuffer[i] == '\r') {
            gpsbuffer[i] = 0;
            return;
		}			
	}
}


int getlock(void) {
	float time;
  char ns, ew;
	int sample;
	float minutes;
	float degrees;
 if(sscanf(gpsbuffer, "GPGGA,%f,%f,%c,%f,%c,%d", &time, &latitude, &ns, &longitude, &ew, &lock) >= 1) { 
            if(!lock) {
                longitude = 0.0;
                latitude = 0.0;        
                return 0;
            } else {
                if(ns == 'S') {    latitude  *= -1.0; }
                if(ew == 'W') {    longitude *= -1.0; 
								degrees = latitude /100.0f;
								degrees = trunca(degrees);
               // float degrees = trunca(latitude / 100.0f);
                minutes = latitude - (degrees * 100.0f);
                latitude = degrees + minutes / 60.0f;    
                
                //degrees = trunc(longitude / 100.0f * 0.01f);
                degrees = trunca(longitude/ 100.0f);
                minutes = longitude - (degrees * 100.0f);
                longitude = degrees + minutes / 60.0f;
                return 1;
            }
	
}
 }
 }

float trunca(float v) {
    if(v < 0.0) {
        v*= -1.0;
        v = floor(v);
        v*=-1.0;
    } else {
        v = floor(v);
    }
    return v;
}
 
/*
void getGPS(void){
	
}
*/


void READ_adxl(void) {
	int x_msb =0;
	int x_lsb = 0;
	float x;
	float x2;
		//I2C aDRESS OF ACCELEROMETER 0x1D
	const int adxl_read = (0x1D << 1) & 0xFE;
	const int adxl_write = (0x1D << 1) | 0x01;	
	
	char cmd[4];
	char read_buff_x[6] = {0};
	char read_buff_y[6] = {0};		
	char read_buff_z[6] = {0};
	
	I2C_configure();	
	
	//activate peripheral

	    cmd[0]= 0x2A;			//Active register adress
			cmd[1]= 0x01;			//Write 1 to activation register
			I2C_send(adxl_write, cmd, 2, 0);	

	//Get x-acceleration	
	
			cmd[0] = 0x01;				
			I2C_send(adxl_write, cmd, 1, 0);
	    I2C_read(adxl_read, read_buff_x, 6, 1); // read the two-byte echo result
//Convert to x reading with msb and lsb, assume last two digits noise?
		x_msb = (int)(read_buff_x[0]<<6); 
		x_lsb = (int)(read_buff_x[1] >>2);
		x = (float)(((float)(x_msb|x_lsb)) / 4096.0);
	
	 x2 = ((int)(read_buff_x[0] << 6) | (int)(read_buff_x[1] >> 2)) / 4096.0;
	
	//x = (float)((int(read_buff_x[0]<<6)|int(read_buff_x[0]<<6))/4096.0);
	
	//Get y-acceleration
			//	cmd[0] = 0x03;					
				//I2C_send(adxl_write, cmd, 1, 0);	
        // Set pointer to location 2 (first echo)
        //I2C_read(adxl_read, read_buff_y, 2, 1); // read the two-byte echo result

//Get z-acceleration
				//cmd[0] = 0x05;					
				//I2C_send(adxl_write, cmd, 1, 0);	
        // Set pointer to location 2 (first echo)
        //I2C_read(adxl_read, read_buff_z, 2, 1); // read the two-byte echo result
				
				/*
		
				/*read_buff[1] = 0x3 & read_buff[0];
				read_buff[0] &= 0x07<<2;
				read_buff[0] >>= 2;*/
				
		/*if(read_buff[1] == 0x01)
				//side = Front;
				else if(read_buff[1] == 0x02)
				//side = Back;
				else
				//side = NOTKNOWN;
				

				
				*/				 
				I2C_powerDown();	
	}




int main (void) {
  SystemCoreClockUpdate();                      /* Get Core Clock Frequency */
  SysTick_Config(SystemCoreClock/1000);         /* Generate interrupt each 1 ms    */
  
  LED_Config();   
//	uart0_init (41940, 9600);
		uart0_init (41940, 4800);
	
	// UArt_0 initilization
	
	SIM->SCGC5    |= (1UL <<  9) | (1UL <<  13);    //Uart_0 clock and Clock to the port (e.g. B/E(13))
	SIM->SCGC5    |= (1UL <<  13);									//Uart_0 clock speed
  SIM->SCGC4		|= (1UL << 10); 									//Enable Uart_0
	
	PORTE->PCR[20] = (0x4 << 8);		//Set UART0 pins to alternative 4 TX
 	PORTE->PCR[21] = (0x4 << 8);		//Set UART0 pins to alteratnative 4 RX
	
	//I2C initiliziation
	
	SIM->SCGC5    |= (1UL <<  10);				//Enable clock for I2C register on port B
	SIM->SCGC4    |= (1UL <<  6);					//Enable I2C Clock
	
	PORTB->PCR[0] = (0x2 <<  8); 					//set adxl scl	set port PTB0 as alternative 2
	PORTB->PCR[1] = (0x2 <<  8); 					//set adxl sda	set port PTB1 as alternative 2
	
	
	//SPI initilization
	
	 spi_init();
		

  
  while(1) {
		//uart0_putchar('A');
		READ_adxl();

    LED_On ();
    Delay(500);
   // LED_Off();
   // Delay(500);
	//	uart0_putchar('B');

  }
}

