/*
 * GccApplication1.c
 *
 * Created: 23/02/2002 16:56:53
 * Author : NegaoDaPiscina
 */ 
#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>

#define set_bit(Y, bit_x) (Y|=(1<<bit_x))
#define clr_bit(Y, bit_x) (Y&=~(1<<bit_x))
#define cpl_bit(Y, bit_x) (Y^=(1<<bit_x))
#define tst_bit(Y, bit_x) (Y&(1<<bit_x))

#define LED PB5

int main(void)
{
    /* Replace with your application code */
	DDRD = 0xFF;
    while (1) 
    {
		cpl_bit(PORTB, LED);
		_delay_ms(200);
    }
}

