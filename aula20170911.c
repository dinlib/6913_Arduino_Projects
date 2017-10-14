#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define set_bit(Y, bit_x) (Y|=(1<<bit_x))
#define clr_bit(Y, bit_x) (Y&=~(1<<bit_x))
#define cpl_bit(Y, bit_x) (Y^=(1<<bit_x))
#define tst_bit(Y, bit_x) (Y&(1<<bit_x))

#define LED1 PB7
#define LED2 PB6

ISR(INT0_vect);
ISR(INT1_vect);

ISR(INT0_vect)
{
	cpl_bit(EIMSK, INT0);
	sei();
	cpl_bit(PORTD, LED2);
	_delay_ms(300);
	cpl_bit(PORTD, LED2);
	cpl_bit(EIMSK, INT0);
	cli();
}

ISR(INT1_vect)
{
	cpl_bit(PORTD, LED2);
	_delay_ms(200);	
	cpl_bit(PORTD, LED2);
}

int main(void)
{
	DDRD = 0b0000000; //PORTD como entrada
	PORTD = 0b00000000; //Estado incial em 0
	DDRB = 0b01100000; //Pinos 6 e 7 da PORTB como saida
	sei();
	EICRA = 0b00000000; //Interrupcoes externas ocorrem em tensao baixa
	EIMSK = 0b00000011; //Habilita interrupcoes externas INT0 e INT1
	while (1)
	{
		cpl_bit(PORTD, LED1);
		_delay_ms(200);	
	}
}

