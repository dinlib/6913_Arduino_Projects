#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define set_bit(Y, bit_x) (Y|=(1<<bit_x))
#define clr_bit(Y, bit_x) (Y&=~(1<<bit_x))
#define cpl_bit(Y, bit_x) (Y^=(1<<bit_x))
#define tst_bit(Y, bit_x) (Y&(1<<bit_x))

#define LED1 PD7
#define LED2 PD6
#define LED3 PD5

ISR(PCINT1_vect);

ISR(PCINT1_vect)
{
	switch(PINC)
	{
		case 0b00000110: cpl_bit(PORTD, LED1); break;
		case 0b00000101: cpl_bit(PORTD, LED2); break;
		case 0b00000011: cpl_bit(PORTD, LED3); break;
	}
}

int main(void)
{
	DDRC = 0b0000000; //PORTC como entrada
	PORTC = 0b00000111; //Estado incial em 1
	DDRD = 0b01110000; //Pinos 5, 6 e 7 como saida
	sei();
	PCICR = 0b00000010; //Habilita interrupcoes por pino em PORTC
	PCMSK1 = 0b00000111; //Habilita interrupcoes PCINT8, 9 e 10 (pinos)
	while (1){}
}

