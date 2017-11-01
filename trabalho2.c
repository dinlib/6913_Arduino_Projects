#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>

#define set_bit(Y,bit_x) (Y|=(1<<bit_x))
#define clr_bit(Y,bit_x) (Y&=~(1<<bit_x))
#define tst_bit(Y,bit_x) (Y&(1<<bit_x))
#define cpl_bit(Y,bit_x) (Y^=(1<<bit_x))

#define DADOS_LCD PORTD
#define nibble_dados 1
#define CONTR_LCD PORTB
#define E PB0
#define RS PB1
#define TRIGGER PB2
#define ECHO PB3

#define tam_vetor 5
#define conv_ascii 48

#define pulso_enable() _delay_us(1); set_bit(CONTR_LCD, E); _delay_us(1); clr_bit(CONTR_LCD, E); _delay_us(45);

const char mensagem[] PROGMEM = "Distancia(cm):\0";

void cmd_LCD(unsigned char c, char cd);
void inic_LCD_4bits();
void escreve_LCD(char *c);
void escreve_LCD_Flash(const char *c);

void cmd_LCD(unsigned char c, char cd){
  if(cd==0)
    clr_bit(CONTR_LCD, RS);
  else
    set_bit(CONTR_LCD, RS);
   
  #if (nibble_dados)
    DADOS_LCD = (DADOS_LCD & 0x0F) | (0xF0 &(c));
  #else
    DADOS_LCD = (DADOS_LCD & 0xF0)|(0xF0 & (c>>4);
  #endif
  
  pulso_enable();
  
  #if(nibble_dados)
    DADOS_LCD = (DADOS_LCD & 0x0F)|(0xF0 & (c<<4));
  #else
    DADOS_LCD = (DADOS_LCD & 0xF0)|(0x0F & c);
  #endif
  
  pulso_enable();
  
  if((cd==0) && (c<<4))
    _delay_ms(2);
}

void inic_LCD_4bits(){
  clr_bit(CONTR_LCD,RS);
  clr_bit(CONTR_LCD,E);
  
  _delay_ms(20);
  
  #if(nibble_dados)
    DADOS_LCD = (DADOS_LCD & 0x0F)| 0x30;
  #else
    DADOS_LCD = (DADOS_LCD & 0xF0)| 0x03;
  #endif
  
  pulso_enable();
  _delay_ms(5);
  pulso_enable();
  _delay_us(200);
  pulso_enable();
  
  #if(nibble_dados)
    DADOS_LCD = (DADOS_LCD & 0x0F)| 0x20;
  #else
    DADOS_LCD = (DADOS_LCD & 0xF0)| 0x02;
  #endif
  
  pulso_enable();
  cmd_LCD(0x28,0);
  cmd_LCD(0x08,0);
  cmd_LCD(0x01,0);
  cmd_LCD(0x0C,0);
  cmd_LCD(0x80,0);
}

void escreve_LCD(char *c){
  for(; *c!=0;c++)
    cmd_LCD(*c,1);
}

void escreve_LCD_Flash(const char *c){
  for(;pgm_read_byte(&(*c))!=0;c++)
    cmd_LCD(pgm_read_byte(&(*c)),1);
}

float read_distance()
{
  uint16_t cycle_counter = 0;
  float time_us = 0;

  // Envia pulso ao trigger
  set_bit(PORTB, TRIGGER);
  _delay_us(10);
  clr_bit(PORTB, TRIGGER);

  // Espera o pulso chegar ao ECHO
  while(!(PINB && 0b00001000));

  while(PINB && 0b00001000)
  {
    cycle_counter += 3;
  }

  time_us = float(cycle_counter) * 0.063;
  return (time_us / 58.0);
}

int main(){

  // Configurar timer 0
  // Prescaler = 1
  // Modo normal
  TCCR0A = 0b00000000;
  TCCR0B = 0b00000001;

  // Habilitar interrupcao por estouro do TC0 (timer 0)
  TIMSK0 = 0b00000001;

  // Habilitar chave geral de interrupcoes
  sei();

  // Pinos de PORTD como saida para o display de LCD
  DDRD = 0xFF;

  // Pinos de PORTB controlam o LCD e o sensor de ultrassom
  // PB3 definido como entrada para receber o ECHO
  DDRB = 0b11110111;
  inic_LCD_4bits();
  escreve_LCD_Flash(mensagem);
  cmd_LCD(0xC0,0);
  for(;;){}
}

