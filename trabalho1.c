#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define set_bit(Y, bit_x) (Y|=(1<<bit_x))
#define clr_bit(Y, bit_x) (Y&=~(1<<bit_x))
#define cpl_bit(Y, bit_x) (Y^=(1<<bit_x))
#define tst_bit(Y, bit_x) (Y&(1<<bit_x))

// TODO: Consertar esses mapeamentos
const unsigned char displaymap[] PROGMEM = {0b11001111, 0b10010010, 0b10000110, 0b11001100, 0b10100100, 0b10100000};
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
  DDRD = 0b01111111;

  // Pino do botao B1 como entrada (PORTB0)
  DDRB = 0b11111110;

  // Habilita o resistor de pullup de PORTB0
  PORTB = 0b00000001;

  // Habilitar chave geral de interrupcoes
  sei();

  while (1){}
}
