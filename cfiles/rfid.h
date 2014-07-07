//#define F_CPU 147545600
//#include<avr/io.h>
//#include<util/delay.h>
//#include "lcd.h"
// all the above should be defined in the application program which is including this header file.


#include<avr/interrupt.h>

unsigned char arr[12]=" ";
unsigned int i=0;
unsigned int j=0;
unsigned int k=0;// flag to check if any RFID tag has been read

void lcd_port_config (void)
{
	DDRC = DDRC | 0xF7;
	PORTC = PORTC & 0x80;
}


void uart3_init(void)
{
	UCSR3B = 0x00; //disable while setting baud rate
	UCSR3A = 0x20;
	UCSR3C = 0x06;
	UBRR3L = 0x5F;
	UBRR3H = 0x00;
	UCSR3B = 0x98;	
}


SIGNAL(SIG_USART3_RECV) 		// ISR for receive complete interrupt
{
	k=1;			// set flag if interrupt occurs
	arr[j++] = UDR3; 		//making copy of data from UDR1 in 'data' variable

	//UDR3=arr[j++];                      //not echoing
	if(j==12)
	{
		j=0;
		cli();		// once the data is read, disable all interrupts so that data is not over written
	}

}

void rf_display(void)
{

	lcd_wr_command(0x01);
	lcd_string("scanning...");
	_delay_ms(400);		// wait for some time to read the tag
	if(k==1)			// check for flag
	{
		cli();		
		j=0;
		
		lcd_cursor(1,1);
		lcd_string("   WELCOME");
		lcd_cursor(2,1);

		for(i=0;i<12;i++)
		lcd_wr_char(arr[i]);	// display the arr on LCD

		_delay_ms(250);
		lcd_wr_command(0x01);
		k=0;		// reset the flag
		sei();		// enable the interrupts 
	}

	else			// if flag is not set, then no valid data so request again
	{
		lcd_cursor(1,1);
		lcd_string("Scan again...");
		_delay_ms(70);
	}
}
					//Function To Initialize all The Devices
void rf_init_devices()
{
 cli(); 					//Clears the global interrupts
 lcd_port_config();				//Initializes all the ports
 lcd_init();
 uart3_init();				//Initialize UART2 for serial communication
 sei();   
}

					// function returns the arr in string format
unsigned char* rf_return(void)
{
	return arr;
}

