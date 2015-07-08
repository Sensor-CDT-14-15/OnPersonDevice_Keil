#include <MKL25Z4.H>
#include "uart.h"

#define LED_NUM     3                   /* Number of user LEDs                */
volatile uint32_t msTicks;                            /* counts 1ms timeTicks */
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
int main (void) {
  SystemCoreClockUpdate();                      /* Get Core Clock Frequency */
  SysTick_Config(SystemCoreClock/1000);         /* Generate interrupt each 1 ms    */
  
  LED_Config();   
	uart0_init (41940, 9600);
	
	// UArt_0 initilization
	
	SIM->SCGC5    |= (1UL <<  9) | (1UL <<  13);    //Uart_0 clock and Clock to the port (e.g. B/E(13))
	SIM->SCGC5    |= (1UL <<  13);									//Uart_0 clock speed
  SIM->SCGC4		|= (1UL << 10); 									//Enable Uart_0
	
	PORTE->PCR[20] = (0x4 << 8);		//Set UART0 pins to alternative 4 TX
 	PORTE->PCR[21] = (0x4 << 8);		//Set UART0 pins to alteratnative 4 RX

  
  while(1) {
		uart0_putchar('A');
    LED_On ();
   // Delay(500);
    LED_Off();
    Delay(500);
		uart0_putchar('B');
  }
}

