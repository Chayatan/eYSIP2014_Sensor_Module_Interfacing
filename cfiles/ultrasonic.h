#include <avr/interrupt.h>
//#include <util/delay.h>
//#include <math.h>
//#include "lcd.h"
unsigned char ADC_Conversion(unsigned char);
unsigned char ADC_Value;
unsigned char sharp, distance, adc_reading;

void lcd_port_config (void)
{
	DDRC = DDRC | 0xF7; //all the LCD pin's direction set as output
	PORTC = PORTC & 0x80; // all the LCD pins are set to logic 0 except PORTC 7
}
void adc_pin_config (void)
{
	DDRF = 0x00; //set PORTF direction as input
	PORTF = 0x00; //set PORTF pins floating
	DDRK = 0x00; //set PORTK direction as input
	PORTK = 0x00; //set PORTK pins floating
}

void adc_init()
{
	ADCSRA = 0x00;
	ADCSRB = 0x00;		//MUX5 = 0
	ADMUX = 0x20;		//Vref=5V external --- ADLAR=1 --- MUX4:0 = 0000
	ACSR = 0x80;
	ADCSRA = 0x86;		//ADEN=1 --- ADIE=1 --- ADPS2:0 = 1 1 0
}


unsigned char ADC_Conversion(unsigned char Ch)
{
	unsigned char a = 0,b = 0;
	
	if(Ch>7)
	{
		ADCSRB = 0x08;
	}

	Ch = Ch & 0x07;
	ADMUX= 0x20| Ch;
	_delay_ms(150);
	ADCSRA = ADCSRA | 0x40;	    //Set start conversion bit
	_delay_ms(150);
	while((ADCSRA&0x10)==0);	    //Wait for ADC conversion to complete
	b = (ADCL>>6);   //read & adjust ADCL result to read as a right adjusted result
	a = (ADCH<<2);   //read & adjust ADCH result to read as a right adjusted result
	a = a | b;
	ADCSRA = ADCSRA|0x10; //clear ADIF (ADC Interrupt Flag) by writing 1 to it
	ADCSRB = 0x00;
	return a;
}



void ultra(void)
{
	unsigned int sr;
	distance = ADC_Conversion(11);		//Stores the Analog value of front sharp connected to ADC channel 11 into variable "sharp"
	lcd_string("    E Yantra");
		lcd_cursor(2,1);
		lcd_string("Distance= ");			
		lcd_print(2,10,distance,3);
		lcd_string(" cm");
}


void port_init()
{
	
	lcd_port_config();
	adc_pin_config();
}

void us_init_devices()
{
	cli();
	port_init();  
	lcd_init();
	adc_init();
	sei();  
}
unsigned int us_return(void)
{
	return distance;
}
