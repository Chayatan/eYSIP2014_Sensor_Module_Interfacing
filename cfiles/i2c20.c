
#define F_CPU 14745600
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "lcd.h"

#define	SLA_W	0xD2             // Write address for DS1307 selection for writing
#define	SLA_R	0xD3




void lcd_port_config(void)
{
	DDRC = DDRC | 0xF7;
	PORTC = PORTC & 0x80;
}
/*void i2c_config()
{
	DDRD = DDRD | 0xF1;
	PORTD = PORTD & 0x80;
	
}*/



void uart3_init(void)
{
	UCSR3B = 0x00; //disable while setting baud rate
	UCSR3A = 0x20;
	UCSR3C = 0x06;//(1<<USBS3) | (3<<UCSZ30);
	UBRR3L = 0x5F;
	UBRR3H = 0x00;
	UCSR3B = 0x98;//(1<<RXEN3) | (1<<TXEN3);
	
}



unsigned char chartobcd(unsigned char n)
{
	return ((n / 10) << 4) | (n % 10);
}
//TWI initialize
// bit rate:72

void twi_init(void)
{
	TWCR = 0x00;   //disable twi
	TWBR = 0x10; //set bit rate
	TWSR = 0x00; //set prescale
	TWAR = 0x00; //set slave address
	TWCR = 0x04; //enable twi
}
void write_byte(unsigned char data_out,unsigned char address)
{
	TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN);       // send START condition
	while(!(TWCR & (1<<TWINT)));                      // wait for TWINT Flag set
	_delay_ms(10);
	//UDR3=TWDR;

	TWDR = SLA_W;                                     // load SLA_W into TWDR Register
	TWCR  = (1<<TWINT) | (0<<TWSTA) | (1<<TWEN);      // clear TWINT flag to start tramnsmission of slave address
	while(!(TWCR & (1<<TWINT)));                      // wait for TWINT Flag set
	_delay_ms(10);
	//UDR3=TWDR;

	TWDR = address;                                   // send address of register byte want to access register
	TWCR  = (1<<TWINT) | (1<<TWEN);                   // clear TWINT flag to start tramnsmission of address
	//UDR3=TWDR;
	while(!(TWCR & (1<<TWINT)));                      // wait for TWINT Flag set
	_delay_ms(10);
	//UDR3=TWDR;

	TWDR = data_out;chartobcd(data_out);                       // convert the character to equivalent BCD value and load into TWDR
	TWCR  = (1<<TWINT) | (1<<TWEN);                   // clear TWINT flag to start tramnsmission of data byte
	while(!(TWCR & (1<<TWINT)));                      // wait for TWINT Flag set
	_delay_ms(10);

	TWCR = (1<<TWINT) | (1<<TWSTO) | (1<<TWEN);       // send STOP condition
}


unsigned char read_byte(unsigned char address)
{
	unsigned char acc_recv_data;

	TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN);       // send START condition
	while(!(TWCR & (1<<TWINT)));                      // wait for TWINT Flag set
	//lcd_cursor(2,1);
	//lcd_string("start");
	_delay_ms(100);
//	UDR3=TWSR;

	TWDR = SLA_W;									   // load SLA_W into TWDR Register
	TWCR  = (1<<TWINT) | (1<<TWEN);                   // clear TWINT flag to start tramnsmission of slave address
	while(!(TWCR & (1<<TWINT)));                      // wait for TWINT Flag set
	//UDR3=TWDR;
	//lcd_cursor(2,1);
	//lcd_string("SLA W");
	_delay_ms(10);

	TWDR = address;                                   // send address of register byte want to access register
	TWCR  = (1<<TWINT) | (1<<TWEN);                   // clear TWINT flag to start tramnsmission of slave address
	while(!(TWCR & (1<<TWINT)));                      // wait for TWINT Flag set
	//UDR3=TWDR;
	//lcd_cursor(2,1);
	//lcd_string("reg add");
	_delay_ms(10);

	TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN);       // send RESTART condition
	while(!(TWCR & (1<<TWINT)));                      // wait for TWINT Flag set

	//lcd_cursor(2,1);
	//lcd_string("restart");
	_delay_ms(10);
//_delay_ms(3000);
	TWDR = SLA_R;									   // load SLA_R into TWDR Register
	TWCR  = (1<<TWINT) | (0<<TWSTA) | (1<<TWEN);      // clear TWINT flag to start tramnsmission of slave address
	while(!(TWCR & (1<<TWINT)));                      // wait for TWINT Flag set
	//UDR3=TWDR;
	//lcd_cursor(2,1);
	//lcd_string("sla+r");
	_delay_ms(10);

	TWCR  = (1<<TWINT) | (1<<TWEN);                   // clear TWINT flag to read the addressed register
	while(!(TWCR & (1<<TWINT)));                      // wait for TWINT Flag set
	//UDR3=TWDR;
	//lcd_cursor(2,1);
	//lcd_string("data read");
	acc_recv_data = TWDR;
	_delay_ms(10);
//lcd_print(1,1,TWDR,3);
//_delay_ms(5000);

	TWDR = 00;                                        // laod the NO-ACK value to TWDR register
	TWCR  = (1<<TWINT) | (1<<TWEN);                   // clear TWINT flag to start tramnsmission of NO_ACK signal
	while(!(TWCR & (1<<TWINT)));                      // wait for TWINT Flag set
//	UDR3=TWDR;
	//lcd_cursor(2,1);
	//lcd_string("stop and NACK");
	_delay_ms(10);
	
	return(acc_recv_data);                            // return the read value to called function
}

void init_devices()
{
	cli();              // disable all interrupts
	lcd_port_config();  // configure the LCD port
	//i2c_config();
	twi_init();         // configur the I2cC, i.e TWI module
	uart3_init();
	sei();              // re-enable interrupts
	//all peripherals are now initialized
}

int main(void)
{
	signed char xl=0,xh=0,yl=0,yh=0,zl=0,zh=0,x1,x2,y1,y2,z1,z2;
	int sign, x_dec,xdisp,xpre;
    int raw_x=0,raw_y=0,raw_z=0,n=0;
	float xrate,yrate,zrate;
		cli();
		sei();
		lcd_port_config();
		lcd_init();
		
		init_devices();
		//lcd_set_4bit();                // set the LCD in 4 bit mode
		lcd_init();                    // initialize the LCD with its commands
		
		write_byte(0x5F,0x2E);
		write_byte(0x10,0x23);
		write_byte(0x8F,0x20);  // normal mode and enable all x y z axis
	//	write_byte(0x40,0x24);
		
		
    l1: while(1)
    {
		//n++;

        	xl=read_byte(0x28);
			xh=read_byte(0x29);
			  	yl=read_byte(0x2a);
			  	yh=read_byte(0x2b);
			    	zl=read_byte(0x2c);
			    	zh=read_byte(0x2d);
			
			//raw_x=(~(xh-1)) | (~(xl-1))-0x25;
			for(int i=0;i<5;i++)
			{
			raw_x=(xh<<8) | xl;
			raw_x=~(raw_x)+1;
			 
			xrate=(float)raw_x/5.7;
			xpre=xdisp;
			xdisp+=(int)xrate;
			}
			xdisp/=5;			
			
			//x1=((int)xrate &0xFF00)>>8;
			//x2= (int)xrate & 0x00FF;
			
			//UDR3 = x1;
			//UDR3 = x2;
			
			
			/*......raw_y=(yh<<8) | yl;
			raw_y=~(raw_y)+1;
			yrate=((float)raw_y*0.07);
			
				
			lcd_cursor(2,1);
			lcdprintf("%d  ",(int)yrate);
			
			y1=((int)yrate &0xFF00)>>8;
			y2= (int)yrate & 0x00FF;
			
						
			//lcd_print(1,8,(int)yrate,5);
			UDR3 = y1;
			UDR3 = y2;
			
			raw_z=(zh<<8) | zl;
			raw_z=~(raw_z)+1;
			zrate=(float)raw_z*0.07;
        	lcd_cursor(2,8);
        	lcdprintf("%d  ",(int)zrate);
        	
			z1=((int)zrate &0xFF00)>>8;
			z2= (int)zrate & 0x00FF;
			
			//lcd_print(2,1,(int)zrate,5);
			UDR3 = z1;
`				UDR3 = z2;
			
			//_delay_ms(5);
			
			
			if(xdisp<25)
			{lcd_cursor(1,7);
			lcd_string("left  ");}

else if(xdisp>25)
{lcd_cursor(1,7);
lcd_string("right  ");}
else
{
lcd_cursor(1,7);
lcd_string("still  ");	
}



	_delay_ms(1000);*/
						
    }
	goto l1;

}

/*

lcd_cursor(1,1);
lcdprintf("%d    ",xdisp);
if(xpre<xdisp)
{
	lcd_string("less");
	_delay_ms(1000);
}
else{
	lcd_string("more");
	_delay_ms(1000);
}
*/