/* Universidad del Valle de Guatemala
 * Programacion de microcontadorroladores. 
 * Laboratorio4.c
 *
 * Created: 4 de abril, 2024
 * Author : Josue Castro
 */ 
#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

uint8_t CONT = 0;
uint8_t C1 = 0;
uint8_t D1 = 0;
uint8_t D2 = 0;
uint8_t DISPLAY[16] = {63, 6, 91, 79, 102, 109, 125, 7, 127, 111, 119, 124, 57, 94, 121, 113};

void initADC(void);
void initTimer0(void);
void setup(void);
void buton(void);

int main(void)
{
	cli();
	setup();
	initADC();
	initTimer0();
	sei();
	while (1)
	{
		//FUNCION DE BOTON
		buton();
		//COMPARACION ENTRE CONTADOR Y ADC
		if (CONT == ADCH){
			PORTC |= (1 << PORTC5); //SI SON IGUALES
			}else{
			PORTC &=~(1 << PORTC5);	//SI NO SON IGUALES
		}
	}
	return 0;
}

void setup(void){
	UCSR0B = 0;		// DESABILITA TX Y RX
	DDRB = 255;		// PUERTO B COMO SALIDA
	DDRD = 255;		// PUERTO D COMO SALIDA
	PORTC = 6;		// PIN PC0 Y PC1 PULL-UP
	DDRC = 248;		// PIN PC0, PC1 Y PC2 COMO ENTRADA
	
	// VALORES INICIALES
	PORTB = 0;
	PORTD = 0;
	PORTC |= (1 << PORTC3);
	PORTC &=~(1 << PORTC4);
}

void initADC(void){
	ADMUX = 0;					// ADC0
	ADMUX |= (1 << REFS0);		// VREF = AVCC
	ADMUX &= ~(1 << REFS1);		// VREF = AVCC
	ADMUX |= (1 << ADLAR);		// JUSTIFICACION A LA DERECHA
	
	ADCSRA = 0;
	ADCSRA |= (1 << ADEN);		// ENCIENDER EL ADC
	ADCSRA |= (1 << ADIE);		// HABILITAR ISR ADC
	ADCSRA |= (1 << ADPS0);
	ADCSRA |= (1 << ADPS1);
	ADCSRA |= (1 << ADPS2);		// PRESCALES 128 -> 16M = 125kHz
	
	DIDR0 |= (1 << ADC0D);		// DESABILITAR LA ENTRADA DIGITAL PC0
}

void initTimer0(void){
	TCCR0A = 0;		//MODO NORMAL
	TCCR0B = 5;		//PRESCALER
	TIMSK0 = 1;		//HABILITA TOIE0
	TCNT0 = 99;		//VALOR INICIAL
}

void buton(void){
	if ((PINC & (1 << PINC1)) == 0){
		_delay_ms(200);					//VALIDA LA PULSACION
		if ((PINC & (1 << PINC1)) == 2){
			CONT++;						//INCREMENTA
			}else{CONT = CONT;}
		}
		if ((PINC & (1 << PINC2)) == 0)	{
			_delay_ms(200);					//VALIDA LA PULSACION
			if ((PINC & (1 << PINC2)) == 4){
				CONT--;						//DECREMENTA
				}else{CONT = CONT;}
			}
			PORTB = CONT;
		}

		ISR(ADC_vect){
			D1 = (ADCH & 15);		//NIBLE BAJO
			D2 = (ADCH & 240);
			D2 = D2 >> 4;			//NIBLE BAJO
			ADCSRA |= (1 << ADIF);	//LIMPIA LA BANDERA
		}

		ISR(TIMER0_OVF_vect){
			ADCSRA |= (1 << ADSC);			//INICIA EL ADC
			
			PORTC ^= (1 << PORTC3);			//CAMBIA EL VALOR PC3
			PORTC ^= (1 << PORTC4);			//CAMBIA EL VALOR PC4
			if ((PORTC & (1 << PINC3)) == 8){
				PORTD = DISPLAY[D1];		//DISPLAY 1
				}else{
				PORTD = DISPLAY[D2];		//DISPLAY 2
			}
			TCNT0 = 99;						//VALOR INICIAL
		}