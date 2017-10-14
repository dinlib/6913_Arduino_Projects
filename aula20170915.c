#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define set_bit(Y, bit_x) (Y|=(1<<bit_x))
#define clr_bit(Y, bit_x) (Y&=~(1<<bit_x))
#define cpl_bit(Y, bit_x) (Y^=(1<<bit_x))
#define tst_bit(Y, bit_x) (Y&(1<<bit_x))

#define LED1 PB7

//Interrupcao por estouro de TC0
ISR(TIMER0_OVF_vect);

ISR(TIMER0_OVF_vect)
{
	cpl_bit(PORTB, LED1);
}

int main(void)
{
	DDRB = 0b10000000; //Pino 7 da PORTB como saida
	PORTB = 0b10000000;
	sei();
	TIMSK = 0b00000001; //Habilita interrupcoes por estouro de TC0
	while (1){}
}
