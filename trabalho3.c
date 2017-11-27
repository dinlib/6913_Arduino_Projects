/*
  Universidade Estadual de Maringá
  Centro de Tecnologia
  Departamento de Informática
  6913 - Sistemas Digitais
  Professor Rodrigo Calvo

  Alunos: Rafael Cortez Sanchez, RA 82357

  Trabalho 3: Controle de Servo Motor com Botões

  Esse projeto para Arduino envolve o controle de um servo motor através de dois
  botões: cada um desses rotaciona o servo para um determinado sentido.
*/

#define F_CPU 16000000UL
#include <avr/io.h>

#define set_bit(Y, bit_x) (Y|=(1<<bit_x))
#define clr_bit(Y, bit_x) (Y&=~(1<<bit_x))

//TOP = (F_CPU/(N*F_PWM))-1 com N=8 e F_PWM = 50Hz
//Utilizado para configurar um contador para gerar pulsos de no maximo 20ms
#define TOP 39999

//Variacao angular: incremento/decremento aplicado ao OCR1A para alterar a
//largura do pulso enviado ao servo motor. Considerando-se que 0,5ms provocam
//um giro de 45 graus, um giro de 1 grau requer 0,011111111ms de pulso. Se 20ms
//requerem 39999 contagens, 0,011111111ms requerem 22 contagens
#define VARIACAO_ANGULAR 22 //para variar em 1 grau o servo

int main()
{
  uint8_t last_pin_state;
  uint16_t new_ocr1a;
  //Pinos controladores do servo
  DDRB = 0b00000110; //habilita pinos OC1A e OC1B (PB1 e PB2) como saidas
  PORTB = 0b11111001;

  //Pinos com botoes
  DDRD = 0b00000000; //pinos PD2 e PD3 como entradas
  PORTD = 0b00001100;

  //TOP = (F_CPU/(N*F_PWM))-1 com N=8 e F_PWM = 50Hz
  //configura o periodo do PWM (20 ms)
  ICR1 = TOP;

  //configura o TC1 para modo PWM rapido via ICR1, prescaler = 8
  TCCR1A = (1 << WGM11);
  TCCR1B = (1 << WGM13) | (1 << WGM12) | (1 << CS11);

  set_bit(TCCR1A, COM1A1); //ativa o PWM no OC1A, modo de comparacao
                           //nao invertido para desabilitar empregar clr_bit(TCCR1A, COM1A1)
  set_bit(TCCR1A, COM1B1); //como acima, mas para OC1B

  //Estado inicial do servo: rotacao 0 graus: 0,5 ms de pulso
  OCR1A = 8000;
  while(1)
  {
    //Checa se botao 1 foi pressionado
    if(PIND & (1<<PD2))
    {
      new_ocr1a = OCR1A + VARIACAO_ANGULAR;
      if(new_ocr1a <= TOP && new_ocr1a >= 8000)
        OCR1A = new_ocr1a;
    }
    //Checa se botao 2 foi pressionado
    if(PIND & (1<<PD3))
    {
      new_ocr1a = OCR1A - VARIACAO_ANGULAR;
      if(new_ocr1a <= TOP && new_ocr1a >= 8000)
        OCR1A = new_ocr1a;
    }
    //Para evitar captura de ruidos nos botoes
    _delay_ms(100);
  }
}
