/*
	Universidade Estadual de Maringá
	Centro de Tecnologia
	Departamento de Informática
	6913 - Sistemas Digitais
	Professor Rogério Calvo

	Alunos:	Luis Paulo de Araújo Afonso, RA 77429 
		Rafael Cortez Sanchez, RA 82357

	Trabalho 1: Dado digital com display de 7 segmentos

	A implementação abaixo considera o display de 7 segmentos do simulador
	Tinkercad. O circuito criado para essa implementação pode ser acessado
	em:
	https://www.tinkercad.com/things/6WaMPJDVwhb-copy-of-super-leelo-curcan/editel?sharecode=CpFDK-5mgiZt_2NilcRxEYrf70NUjAJtQKUzHgtzrlY=

	Um botão foi colocado em PB0 (pino 8 do Arduino) para gerar uma inter-
	rupção que mostra o valor de "die_value" no display. Essa variável é
	constantemente atualizada por uma interrupção de timer, que a faz ro-
	tacionar entre os valores {1, 2, 3, 4, 5, 6}. Isso cria o comportamen-
	to de um dado virtual, o qual é lançado ao se pressionar o botão liga-
	do a PB0.
*/

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define set_bit(Y, bit_x) (Y|=(1<<bit_x))
#define clr_bit(Y, bit_x) (Y&=~(1<<bit_x))
#define cpl_bit(Y, bit_x) (Y^=(1<<bit_x))
#define tst_bit(Y, bit_x) (Y&(1<<bit_x))

/*
	Mapeamento dos segmentos:
		PD0: inutilizado
		PD1: segmento "a"
		PD2: segmento "b"
		PD3: segmento "c"
		PD4: segmento "d"
		PD5: segmento "e"
		PD6: segmento "f"
		PD7: segmento "g"

*/
const unsigned char displaymap[] PROGMEM = {0b11110011, 0b01001001, 0b01100001, 0b00110011, 0b00100101, 0b00000101};
uint8_t die_value = 0;

// Tratador da interrupcao de timer
ISR(TIMER0_OVF_vect);
// Tratador da interrupcao do botao B1
ISR(PCINT0_vect);

ISR(TIMER0_OVF_vect)
{
  ++die_value;
  if(die_value > 5)
    die_value = 0;
}

ISR(PCINT0_vect)
{
  PORTD = pgm_read_byte(&displaymap[die_value]);
}

int main(void)
{
  // Usar RX como pino de proposito geral
  UCSR0B = 0x00;

  // Configurar timer 0
  // Prescaler = 1
  // Modo normal
  TCCR0A = 0b00000000;
  TCCR0B = 0b00000001;

  // Habilitar interrupcao por estouro do TC0 (timer 0)
  TIMSK0 = 0b00000001;

  // Habilita interrupcoes nos pinos de PORTB
  PCICR = 0b00000001;

  // Habilitar interrupcao do botao B1 (PORTB0)
  PCMSK0 = 0b00000001;

  // Pinos de saida do display de 7 segmentos
  DDRD = 0b11111110;

  // Pino do botao B1 como entrada (PORTB0)
  DDRB = 0b11111110;

  // Habilita o resistor de pullup de PORTB0
  PORTB = 0b00000001;

  // Habilitar chave geral de interrupcoes
  sei();

  while (1){}
}
