
//#define F_CPU 14745600
//#include <avr/io.h>
//#include <util/delay.h>

//include all the above headers in the application program

void PIR(void)
{
	DDRL &=0x00;
	PORTL &= 0x00;
}

unsigned char PIR_scan()
{
	unsigned char x;
	x=PINL &0xFF;
	return x;
}

void LED(void)
{
	DDRJ = 0xFF;
	PORTJ = 0xFF;
}

void bar_led_on()
{
	PORTJ = 0xFF;
}

void bar_led_off()
{
	PORTJ = 0x00;
}

void PIR_initialize(void)
{
	void PIR();
	void LED();
}