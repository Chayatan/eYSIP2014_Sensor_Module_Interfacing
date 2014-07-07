/*
 * ultra_head.c
 *
 * Created: 6/4/2014 5:41:39 PM
 *  Author: ERTS Lab
 */ 


#define F_CPU 147545600
#include<avr/io.h>
//#include<avr/interrupt.h>
#include<util/delay.h>
#include "lcd.h"
#include "ultrasonic.h"

int main(void)
{
	us_init_devices();
	unsigned char d;
    while(1)
    {
		
	_delay_ms(150);
	ultra();
	_delay_ms(150);   //TODO:: Please write your application code 
  	d=us_return();
	lcd_wr_command(0x01);
	lcd_print(1,1,d,3);
    }
return 0;
}