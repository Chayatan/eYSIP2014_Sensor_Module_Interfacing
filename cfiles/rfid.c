#define F_CPU 147545600
#include<avr/io.h>
#include<avr/interrupt.h>
#include<util/delay.h>
#include "lcd.h"

unsigned char arr[12];
unsigned int j=0;

void lcd_port_config (void)
{
	DDRC = DDRC | 0xF7; 
	PORTC = PORTC & 0x80; 
}


void uart3_init(void)
{
	UCSR3B = 0x00; 		//disable while setting baud rate
	UCSR3A = 0x20; 		//UDREn Flag indicates if it is ready to receive new data
	UCSR3C = 0x06; 		//8 bit 0 parity bits and 1 stop bit
	UBRR3L = 0x5F; 		//set baud rate to 9600bps
	UBRR3H = 0x00; 
	UCSR3B = 0x98;		// enable the interrupts
	
}
	

SIGNAL(SIG_USART3_RECV) 		// ISR for receive complete interrupt
{
	arr[j] = UDR3; 				//making copy of data from UDR3 in 'data' variable 

	UDR3=arr[j++];				//echoing back. you can see it on the PC
	if(j==12)					// counter to check if all 12 bytes are received
	{
		j=0;					// reset counter
		lcd_cursor(1,1);
		lcd_string("   WELCOME");
		lcd_cursor(2,1);
		lcd_string(arr);
		_delay_ms(500);
		lcd_wr_command(0x01);	// clear display
	}

}

//Function To Initialize all The Devices

void rf_init_devices()
{
	 cli();							//Clears the global interrupts
	 lcd_port_config();				//Initializes all the ports
	 lcd_init();					// initialize LCD for 2x16 display
	 uart3_init();					//Initialize UART3 for serial communication
	 sei();							// enable all interrupts
}

int main(void)
{
	rf_init_devices();
	
	while(1);
	
	return 0;
}	
