

#include<avr/io.h>
#define F_CPU	9600000//9600000
//#include<util/delay.h>
#include <util/delay.h>
#define TIME 100000 //shutdown80000// 720000000 para probar bateria
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

//PBO ENTRADA IGNICIÓN
//PB1 SALIDA CONMUTADOR DE VOLTAJE
//PB2 ENTRADA ANALOGA DIGITAL
//PB3 SALIDA CORE/LED IGNICIÓN
//PB4 ENTRDA SEÑAL DE VIDA
//PB5 RESET

long timeout_off = TIME;
long timeout_on = TIME;
long read = 0;
long read_old = 0;
long timeout_batt = 0;
long tiempo_inc = 0;

int main(void)
{
	PORTB = 0x00;  //bit 5 para regulador modem //10
	DDRB =  0x1B;  // 3B
	_delay_ms (1000);	
	adc_setup();
	PORTB =0x31;
	_delay_ms (1000);
	PORTB =0x30;
	_delay_ms (1000);
	PORTB =0x31;
	
	while (1)
	{
		//PORTB = 0x11;
		//DDRB =  0x1B;
		while (adc_read() < 17)  //33
		{
			PORTB = 0x00;  //20
		}
// 	PORTB = 0x31;
// 	_delay_ms (1000); 
// 	PORTB =0x11;
// 	_delay_ms (1000);
	//PORTB =0x13;
 	//_delay_ms (500);
	//PORTB =0x13;
		timeout_off = TIME;
		while(timeout_off >= 1)
		{
			read_old = read;
			read = adc_read();
			if ((read > 16) && (read_old > 16)) //31
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
			if (read <= 16)   //32
			{
				timeout_off --;
				timeout_batt = TIME_BAT;
				PORTB = 0x12;  ///12 
			}		
		}

		// Conmuta la salida del módulo a alimentación con bateria
		PORTB = 0x1A; //1A
		// Espera 15 Segundos
		_delay_ms(15000); //15000
		// Corta la salida.
		//PORTB = 0x20;  //apagar regulador modem ojo
		//DDRB =  0x1B; //Z8
		PORTB = 0x00; 
		_delay_ms(2000);
	}
}