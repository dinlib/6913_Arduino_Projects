#define F_CPU 16000000UL
#include <avr/io.h>

#define set_bit(Y, bit_x) (Y|=(1<<bit_x))
#define clr_bit(Y, bit_x) (Y&=~(1<<bit_x))

#define TOP 39999

ISR(INT0_vect);
ISR(INT1_vect);

ISR(INT0_vect)
{
	OCR1A += 100;
}

ISR(INT1_vect)
{
	OCR1A -= 100;
}

int main()
{
	DDRB = 0b00000110; //habilita pinos OC1A e OC1B (PB1 e PB2) como saidas
	PORTB = 0b11111001;

	PORTD = 0b00000000;

	//TOP = (F_CPU/(N*F_PWM))-1 com N=8 e F_PWM = 50Hz
	//configura o periodo do PWM (20 ms)
	ICR1 = TOP;

	//configura o TC1 para modo PWM rapido via ICR1, prescaler = 8
	TCCR1A = (1 << WGM11);
	TCCR1B = (1 << WGM13) | (1 << WGM12) | (1 << CS11);

	set_bit(TCCR1A, COM1A1); //ativa o PWM no OC1A, modo de comparacao
				 //nao invertido para desabilitar empregar clr_bit(TCCR1A, COM1A1)
	set_bit(TCCR1A, COM1B1); //como acima, mas para OC1B

	OCR1A = TOP;
	EICRA = 0b00000000; //Interrupcoes externas ocorrem em tensao baixa
	EIMSK = 0b00000011; //Habilita interrupcoes externas INT0 e INT1
	sei();
	while(1) {}
}
