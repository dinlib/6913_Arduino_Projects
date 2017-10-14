#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define set_bit(Y, bit_x) (Y|=(1<<bit_x))
#define clr_bit(Y, bit_x) (Y&=~(1<<bit_x))
#define cpl_bit(Y, bit_x) (Y^=(1<<bit_x))
#define tst_bit(Y, bit_x) (Y&(1<<bit_x))

#define LED1 PB1
#define LED2 PB2

//Interrupcoes para timer
//ISR(timer0_compa_vect);
//ISR(timer0_compb_vect);

int main(void)
{
	//Modo de operacao PWM rapido de 10 bits
	//Valor maximo da contagem em ICR1
	//Prescaler de 1024 no clock interno
        TCCR1A = 0b01010011;
        TCCR1B = 0b00001101;
	
	//Habilitar interrupcoes
	TIMSK1 = 0b00000001;
	sei()
	
	//Pinos 1 e 2 da PORTB como saida
        DDRB = 0b00000110;
        while (1){}
}
