/*----------------------------------------------------------------
--------------------SOURCE OF ELECDUDE.COM------------------------
-----------------------------------------------------------------*/

#ifndef USART_H
#define USART_H

/*----------------------------------------------------------------
-------------------HEADER FILES to be included--------------------
-----------------------------------------------------------------*/
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include "USART.h"

/*----------------------------------------------------------------
-----------------------constants and macros-----------------------
-----------------------------------------------------------------*/
#define BIT(x)			(1 << (x))			//Set a particular bit mask
#define CHECKBIT(x,b) 	(x&b)			//Checks bit status
#define SETBIT(x,b) 	x|=b;			//Sets the particular bit
#define CLEARBIT(x,b) 	x&=~b;		//Sets the particular bit
#define TOGGLEBIT(x,b) 	x^=b;		//Toggles the particular bit

#define uart_puts_P(__s)       uart_puts_p(PSTR(__s))

/** @brief  UART Baudrate Expression for ATmega
 *  @param  xtalcpu  system clock in Mhz, e.g. 4000000L for 4Mhz          
 *  @param  baudrate baudrate in bps, e.g. 1200, 2400, 9600     */
#define UART_BAUD_SELECT(baudRate,xtalCpu) ((xtalCpu)/((baudRate)*16l)-1)

//* @brief  UART Baudrate Expression for ATmega double speed mode
#define UART_BAUD_SELECT_DOUBLE_SPEED(baudRate,xtalCpu) (((xtalCpu)/((baudRate)*8l)-1)|0x8000)

#define UART_FRAME_ERROR      0x0800              /* Framing Error by UART       */
#define UART_OVERRUN_ERROR    0x0400              /* Overrun condition by UART   */
#define UART_NO_DATA          0x0200              /* no receive data available   */

/*------------------------------------------------------------------------
---------------------------FUNCTIONS--------------------------------------
--------------------------------------------------------------------------
Function: uart_init()
Purpose:  initialize UART and set baudrate
Input:    baudrate using macro UART_BAUD_SELECT()
Returns:  none
**************************************************************************/
extern void uart_init(unsigned int baud)
{
	unsigned int baudrate;
	baudrate=UART_BAUD_SELECT(baud,F_CPU);
	CLEARBIT(DDRD,BIT(0))
	SETBIT(DDRD,BIT(1))
	CLEARBIT(PORTD,BIT(0) | BIT(1))

	UCSRB = 0x00; //disable while setting baud rate
	UCSRA = 0x00;
	UCSRC = (1 << URSEL) | 0x06; 
/* Set frame format: asynchronous, 8data, no parity, 1stop bit */
	
	UBRRH = (unsigned char)(baudrate>>8);
    UBRRL = (unsigned char) baudrate;

	UCSRB = 0x18;	//Enable the USART receiver and transmitter.	
}


/*************************************************************************
Function: uart_getc()
Purpose:  return byte from ringbuffer  
Returns:  lower byte:  received byte from ringbuffer
          higher byte: last receive error
**************************************************************************/
extern unsigned int uart_getc(void)
{    
   unsigned char data;
   unsigned char status;
   unsigned char lastRxError;

   while(!(UCSRA & (1<<RXC))); 	// Wait for incomming data

   /* read UART status register and UART data register */ 
   status = UCSRA;
   data = UDR;

   lastRxError = (status & (_BV(FE)|_BV(DOR)) );

   return (lastRxError << 8) + data;
}

/*************************************************************************
Function: uart_gets()
Purpose:  return set of bytes from ringbuffer  
Returns:  lower byte:  received byte from ringbuffer
          higher byte: last receive error
**************************************************************************/
extern void uart_gets(char *s,int size)
{    
   unsigned char data;
   unsigned char status;
   unsigned char lastRxError;

	while(size>=0)
	{
	   	*s=uart_getc();
		s++;
		size--;
	}
}


/*************************************************************************
Function: uart_putc()
Purpose:  write byte to ringbuffer for transmitting via UART
Input:    byte to be transmitted
Returns:  none          
**************************************************************************/
extern void uart_putc(unsigned char data)
{
	while ( !(UCSRA & (1<<UDRE)) );  /* Wait for empty transmit buffer */
	
	UDR = data; 			         /* Start transmition */
}

/*************************************************************************
Function: uart_puts()
Purpose:  transmit string to UART
Input:    string to be transmitted
Returns:  none          
**************************************************************************/
extern void uart_puts(const char *s )
{
    while (*s) 
      uart_putc(*s++);

}

/*************************************************************************
Function: uart_puts_p()
Purpose:  transmit string from program memory to UART
Input:    program memory string to be transmitted
Returns:  none
**************************************************************************/
extern void uart_puts_p(const char *progmem_s )
{
    register char c;
    
    while ( (c = pgm_read_byte(progmem_s++)) ) 
      uart_putc(c);

}

/*************************************************************************
Function: uart_initDouble()
Purpose:  initialize UART and set DOUBLE baudrate
Input:    baudrate using macro UART_BAUD_SELECT_DOUBLE_SPEED()
Returns:  none
**************************************************************************/
/*extern void uart_initDouble(unsigned int baud)
{
	unsigned int baudrate;
	baudrate=UART_BAUD_SELECT_DOUBLE_SPEED(baud,F_CPU);
	CLEARBIT(DDRD,BIT(0))
	SETBIT(DDRD,BIT(1))
	CLEARBIT(PORTD,BIT(0) | BIT(1))

	UCSRB = 0x00; //disable while setting baud rate
	UCSRA = 0x00;
	UCSRC = (1 << URSEL) | 0x06; 
// Set frame format: asynchronous, 8data, no parity, 1stop bit
	
	UBRRH = (unsigned char)(baudrate>>8);
    UBRRL = (unsigned char) baudrate;

	UCSRB = 0x18;	//Enable the USART receiver and transmitter.	
}
*/

#endif // UART_H 
