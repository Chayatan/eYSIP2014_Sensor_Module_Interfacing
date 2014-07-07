#define F_CPU 147545600
#include<avr/io.h>
#include<util/delay.h>
#include "lcd.h"
#include "rfid.h"

void main(void)
{
	rf_init_devices();
	lcd_cursor(2,4);
	_delay_ms(50);
	rf_display();
	cli();
	lcd_wr_command(0x01);
	lcd_cursor(2,4);
	//while(1);
	return 0;
}
