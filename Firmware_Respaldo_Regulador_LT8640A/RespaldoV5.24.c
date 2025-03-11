//           PROYECTO SIBUS 2025

#include<avr/io.h>
#define F_CPU	9600000 //9600000
#include <util/delay.h>
#define TIME 100000 //shutdown80000// 720000000 para probar bateria
#define TIME_BAT 1000

void adc_setup (void)
{
	// Set the ADC input to PB2/ADC1
	ADMUX |= (1 << MUX0);
	// Desactivar ADLAR para permitir la lectura de 10 bits
	ADMUX &= ~(1 << ADLAR);
	// Set the prescaler to clock/128 & enable ADC
	ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (0 << ADPS0) | (1 << ADEN);
}

int adc_read (void)
{
	// Start the conversion
	ADCSRA |= (1 << ADSC);

	// Wait for it to finish
	while (ADCSRA & (1 << ADSC));

	// Retornar el valor de 10 bits (se obtiene de los registros ADCL y ADCH)
	return ADC; // ADC contiene los 10 bits combinados
}

// Variables y configuraciones de la lógica
long timeout_off = TIME;
long timeout_on = TIME;
long read = 0;
long read_old = 0;
long timeout_batt = 0;
long tiempo_inc = 0;

int main(void)
{
	PORTB = 0x00;  // bit 5 para regulador modem //10
	DDRB =  0x1B;  // 3B
	_delay_ms(1000);
	adc_setup();
	PORTB = 0x31;
	_delay_ms(1000);
	PORTB = 0x30;
	_delay_ms(1000);
	PORTB = 0x31;
	
	//Fomrula para conocer el voltaje: V = (valor_ref * (2/39.4) * (3.3/1023)) 
	// valor_ref = valor que se desea encontrar para que sea leído por el adc.
	// 0.6V = Voltaje de caída del diodo

	while (1)
	{
		// Espera que el valor leído del ADC sea mayor que un umbral
		while (adc_read() < 120)  // 33 // 17
		{
			PORTB = 0x00;  // 20
		}

		timeout_off = TIME;
		while(timeout_off >= 1)
		{
			read_old = read;
			read = adc_read();

			// Compara si ambos valores de lectura están por encima del umbral
			if ((read > 121) && (read_old > 121)) // 31 // 16 // 17
			{
				timeout_off = TIME;
				if (timeout_batt == 0)
				{
					PORTB = 0x11;
				}
				else
				{
					timeout_batt--;
				}
			}

			// Si el valor leído es menor o igual al umbral
			if (read <= 121)   // 32
			{
				timeout_off--;
				timeout_batt = TIME_BAT;
				PORTB = 0x12;  /// 12
			}
		}

		// Conmuta la salida del módulo a alimentación con batería
		PORTB = 0x1A; // 1A
		// Espera 15 Segundos
		_delay_ms(15000); // 15000
		// Corta la salida.
		PORTB = 0x00;
		_delay_ms(2000);
	}
}
