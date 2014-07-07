
#define F_CPU 14745600
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <util/lcd.h>

void lcd_port_config (void)
{
	DDRC = DDRC | 0xF7; //all the LCD pin's direction set as output
	PORTC = PORTC & 0x80; // all the LCD pins are set to logic 0 except PORTC 7
}

void PIR(void)
{
	DDRL &=0x00;
	PORTL &= 0x00;
}

//Main Function
int main(void)
{
lcd_port_config();

lcd_init();
	unsigned char x;
	
	PIR();
	
	while(1)
	{
		
		x = PINL &0xFF;
		
		if(x) //switch is not pressed
		{
			
			lcd_cursor(1,1);
			lcd_string("Human Detected    ");
		}
		else
		{
			lcd_cursor(1,1);
			lcd_string("Empty Space    ");
		}
		
	}
}
