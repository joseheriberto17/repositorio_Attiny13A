

#include<avr/io.h>
#define F_CPU	9600000
#include<util/delay.h>
#define TIME 45000
#define TIME_BAT 1000

/* Comentar las lineas que no correspondan a la version que se pretende compilar.
V10 para las tarjetas de la primera version.
V20 para las tarjetas de la segunda version.
V30 y V40 para versiones futuras si se requiere.
*/


void adc_setup (void)
{
	// Set the ADC input to PB2/ADC1
	ADMUX |= (1 << MUX0);
	ADMUX |= (1 << ADLAR);

	// Set the prescaler to clock/128 & enable ADC
	ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (0 << ADPS0) | (1 << ADEN);
}

int adc_read (void)
{
	// Start the conversion
	ADCSRA |= (1 << ADSC);

	// Wait for it to finish
	while (ADCSRA & (1 << ADSC));

	return ADCH;
}

long timeout_off = TIME;
long timeout_on = TIME;
long read = 0;
long timeout_batt = 0;
int main(void)
{
	// Inicializa puertos con las salidas en cero
	PORTB = 0x00;
	DDRB =  0x1B;
	adc_setup();


	while (1)
	{
		while (adc_read() < 33)
		{
		}
		PORTB = 0x01;

		timeout_off = TIME;
		while(timeout_off >= 1)
		{
			read = adc_read();
			
			
			if (read > 30)
			{
				timeout_off = TIME;

				if (timeout_batt == 0)
				{
					PORTB = 0x11;
				}
				else
				{
					timeout_batt --;
				}				

			}
			else
			{
				timeout_off --;
			}
			
			if (read < 27)
			{
				PORTB = 0x13;
				timeout_batt = TIME_BAT;
			}
		}



		// Conmuta la salida del módulo a alimentación con bateria
		PORTB = 0x1B;

		// Espera 15 Segundos
		_delay_ms(15000);
		
		// Corta la salida.
		PORTB = 0x00;
		_delay_ms(2000);
		//		PORTB = 0x01;
		


	}


}