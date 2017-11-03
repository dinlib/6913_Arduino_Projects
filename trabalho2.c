/*
  Universidade Estadual de Maringá
  Centro de Tecnologia
  Departamento de Informática
  6913 - Sistemas Digitais
  Professor Rogério Calvo

  Alunos: Rafael Cortez Sanchez, RA 82357
	  Andre Dias Menegazzo Pereira, RA 47735

  Trabalho 2: Sensor de distancia com ultrassom

  A implementação abaixo utiliza um sensor de distância por ultrassom ligado a
  uma placa Arduino Uno. Ao ser acionado, o módulo de sensor envia um pulso à
  porta PB0 do microcontrolador, sendo que a distância percebida pelo sensor é
  proporcional à largura desse pulso.

  Para calcular a largura do pulso recebido pelo sensor, foi configurado o
  timer 1 da MCU para gerar interrupções por captura de sinal, tanto para
  subida quanto para descida desse. O intervalo entre esses eventos permite
  calcular a largura do pulso em contagens do timer, que podem ser convertidas
  em tempo, considerando-se que a frequência de 16MHz do clock interno é
  constante. Com a largura do pulso em microssegundos, enfim, calcula-se a
  distância entre o objeto percebido e o sensor.

  O valor de distância obtido pelo sensor é mostrado em um display de LCD 2x16.
  Além disso, três LEDs indicam níveis discretos de proximidade do objeto: O
  LED1 acende quando o objeto está mais próximo que uma distância d1, o LED2
  acende quando o objeto está entre d1 e d2, e o LED3 acende quando o objeto
  está além de d2. Se o objeto está fora do alcance do sensor, acende-se o LED
  3.
*/

#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <stdio.h>

#define set_bit(Y,bit_x) (Y|=(1<<bit_x))
#define clr_bit(Y,bit_x) (Y&=~(1<<bit_x))
#define tst_bit(Y,bit_x) (Y&(1<<bit_x))
#define cpl_bit(Y,bit_x) (Y^=(1<<bit_x))

#define DADOS_LCD PORTD
#define nibble_dados 1
#define CONTR_LCD PORTB
#define ECHO PB0
#define TRIGGER PB1
#define RS PB2
#define E PB3
#define LED1 PD1
#define LED2 PD2
#define LED3 PD3

#define DISTANCIA_MEDIA_BEGIN 70
#define DISTANCIA_MEDIA_END 120

#define pulso_enable() _delay_us(1); set_bit(CONTR_LCD, E); _delay_us(1); clr_bit(CONTR_LCD, E); _delay_us(45);
#define pulso_trigger() set_bit(PORTB, TRIGGER); _delay_us(15); clr_bit(PORTB, TRIGGER);

const char mensagem[] PROGMEM = "Distancia(mm):\0";
uint16_t inicio_pulso = 0;
uint16_t fim_pulso = 0;
uint8_t quant_overflow = 0;
uint32_t largura_de_pulso = 0;

void cmd_LCD(unsigned char c, char cd);
void inic_LCD_4bits();
void escreve_LCD(char *c);
void escreve_LCD_Flash(const char *c);

//Overflow do contador 1
ISR(TIMER1_OVF_vect)
{
  ++quant_overflow;
}

// Interrupcao na captura de TC1 (ECHO do módulo de ultrassom)
ISR(TIMER1_CAPT_vect)
{
  //borda de subida
  if (tst_bit(PINB, ECHO))
  {
    //salvar o tempo de inicio do pulso
    inicio_pulso = ICR1;

    //mudar interrupcao para borda de descida
    TCCR1B = TCCR1B & 0xBF;

    //resetar contador de overflow do TC1
    quant_overflow = 0;
  }
  //borda de descida
  else
  {

    //salvar tempo do fim do pulso
    fim_pulso = ICR1;

    //mudar interrupcao para borda de subida
    TCCR1B = TCCR1B | 0x40;

    //calcula largura de pulso em contagens do contador
    largura_de_pulso = (uint32_t)fim_pulso -
                       (uint32_t)inicio_pulso +
                       (uint32_t)(quant_overflow * 65536);

    //largura de pulso em microssegundos
    //considerando frequencia de clock de 16MHz
    largura_de_pulso /= 16;
  }
}

// Envia comandos para o LCD
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

// Inicializa LCD com interface de 4 bits
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

// Escreve mensagem no LCD a partir do cursor
void escreve_LCD(char *c){
  for(; *c!=0;c++)
    cmd_LCD(*c,1);
}

// Como escreve_LCD, mas para strings contidas na memoria flash
void escreve_LCD_Flash(const char *c){
  for(;pgm_read_byte(&(*c))!=0;c++)
    cmd_LCD(pgm_read_byte(&(*c)),1);
}

// Acende os LEDs de distancia
void set_led_distancia(uint16_t distancia_mm)
{
  if(distancia_mm >= DISTANCIA_MEDIA_BEGIN &&
     distancia_mm <= DISTANCIA_MEDIA_END)
    {
      clr_bit(PORTD, LED1);
      set_bit(PORTD, LED2);
      clr_bit(PORTD, LED3);
    }
  else if(distancia_mm > DISTANCIA_MEDIA_END)
  {
    clr_bit(PORTD, LED1);
    clr_bit(PORTD, LED2);
    set_bit(PORTD, LED3);
  }
  else
  {
    set_bit(PORTD, LED1);
    clr_bit(PORTD, LED2);
    clr_bit(PORTD, LED3);
  }
}

int main()
{
  uint16_t distancia_mm;
  char distancia_texto[16];
  // Configurar timer 1 em modo normal
  // Prescaler = 1
  // Com cancelador de ruido para captura
  // Dispara evento de captura na borda de subida
  TCCR1A = 0b00000000;
  TCCR1B = 0b11000001;

  // Habilitar interrupcao por estouro do TC1 (timer 1)
  // Habilitar interrupcao de TC1 por captura de entrada
  TIMSK1 = 0b00100001;

  // Habilitar chave geral de interrupcoes
  sei();

  // Pinos de PORTD como saida para o display de LCD e para os LEDs de distancia
  DDRD = 0xFF;

  // Pinos de PORTB controlam o LCD e o sensor de ultrassom
  // PB0 definido como entrada para receber interrupcoes TC1 do ECHO (ultrassom)
  DDRB = 0b11111110;

  // Inicializa display e escreve mensagem inicial "Distancia(mm):"
  inic_LCD_4bits();
  escreve_LCD_Flash(mensagem);
  for(;;)
  {
    pulso_trigger(); // Envia pulso ao trigger
    _delay_ms(500); // Tempo grande para facilitar visualizacao do display e dos leds
    distancia_mm = (uint16_t)((double)largura_de_pulso / 5.8);
    sprintf(distancia_texto, "%015d", distancia_mm);
    cmd_LCD(0xC0,0); // Posiciona cursor no comeco da segunda linha
    escreve_LCD(distancia_texto); // Escreve distancia
    set_led_distancia(distancia_mm); // Aciona led de distancia
  }
}
