#define F_CPU 147545600
#include<avr/io.h>
#include<util/delay.h>
#include <string.h>
#include "lcd.h"
#include "rfid.h"

void main()
{
	unsigned char tag1[] = "4100A352E959";	// This is the valid tag the system is looking for
	unsigned char *str;
	rf_init_devices();

	while(1)
	{
		_delay_ms(50);
		rf_display();		// read the tag, store it and display
		str = rf_return();		// get the read tag details into another string
		lcd_cursor(1,1);
		lcd_string(str);
		_delay_ms(100);
	
		if(!strcmp(str,tag1))		// compare the required tag with the tag which is read
		{
			lcd_cursor(2,1);
			lcd_string(" Valid Tag");
		}	
		else
		{
			lcd_cursor(2,1);
			lcd_string(" invalid tag");
		}	

		_delay_ms(100);
		lcd_wr_command(0x01);
	}
	
	return 0;
	
}