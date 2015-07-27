#include <MKL25Z4.H>
#include "spi.h"


/*  
int main(void)  
{  
  
  char ch;  
  
  spi_init();   //Init SPI0  
  
  while(1)  
 {  
   ch = in_char();  //Read char from terminal  
   spi_send(ch);    //Send char over SPI  
   out_char(ch);    //Echo char to terminal  
     
  }  
  
}  

*/
  
	
 
void spi_init(void)  
{  
 SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK;      //Turn on clock to D module  
 SIM->SCGC4 |= SIM_SCGC4_SPI0_MASK;       //Enable SPI0 clock  
  
  PORTD->PCR[0] = PORT_PCR_MUX(0x2);    //Set PTD0 to mux 2 [SPI0_PCS0]  
  PORTD->PCR[1] = PORT_PCR_MUX(0x2);    //Set PTD1 to mux 2 [SPI0_SCK]  
  PORTD->PCR[2] = PORT_PCR_MUX(0x2);    //Set PTD2 to mux 2 [SPI0_MOSI]  
  PORTD->PCR[3] = PORT_PCR_MUX(0x2);    //Set PTD3 to mux 2 [SPIO_MISO]  
  
  SPI0->C1 = SPI_C1_MSTR_MASK | SPI_C1_SSOE_MASK;   //Set SPI0 to Master & SS pin to auto SS  
    
  SPI0->C2 = SPI_C2_MODFEN_MASK;   //Master SS pin acts as slave select output   
    
  SPI0->BR = (SPI_BR_SPPR(0x02) | SPI_BR_SPR(0x08));     //Set baud rate prescale divisor to 3 & set baud rate divisor to 64 for baud rate of 15625 hz  
    
  SPI0->C1 |= SPI_C1_SPE_MASK;    //Enable SPI0  
  
}  
  
void spi_send(char spiMsg)  
{  
  
  while(!(SPI_S_SPTEF_MASK & SPI0->S))  
  {      
    asm("nop");  //While buffer is not empty do nothing  
  }  
      
  SPI0->D = spiMsg;    //Write char to SPI  
  
}  