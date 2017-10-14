#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define set_bit(Y, bit_x) (Y|=(1<<bit_x))
#define clr_bit(Y, bit_x) (Y&=~(1<<bit_x))
#define cpl_bit(Y, bit_x) (Y^=(1<<bit_x))
#define tst_bit(Y, bit_x) (Y&(1<<bit_x))

#define LED1 PD5
#define LED2 PD6

//Interrupcoes para timer
//ISR(timer0_compa_vect);
//ISR(timer0_compb_vect);

int main(void)
{
        TCCR0A = 0b01010010;
        TCCR0B = 0b00000101;
        OCR0A = 500;
        OCR0B = 200;
	//TIMSK0 = 0b00000110 //Habilitar interrupcoes nos contadores OCR0A e B
	//sei()
        DDRD = 0b01100000; //Pinos 5 e 6 da PORTD como saida (OC0A e OC0B)
        while (1){}
}
