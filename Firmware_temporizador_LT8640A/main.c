#include <avr/io.h>
#define F_CPU	9600000
#include <util/delay.h>
#define TIME 45000      // 45000 aproximadamente 5 segundos // 10.800.000 19 minutos con 40 segundos 10800000
#define REST 5400000       // 1080000 aproximadamente 2 minutos // 540000 para 1 minuto
#define LED_IGN PB3
#define VOLT_OUT PB1 
#define TIME_PWM 50   ///4 milisegundos
#define TIME_PULSOS 2160000 // 3 minutos

//PBO ENTRADA IGNICIÓN
//PB1 SALIDA CONMUTADOR DE VOLTAJE
//PB2 ENTRADA ANALOGA DIGITAL
//PB3 SALIDA CORE/LED IGNICIÓN
//PB4 ENTRDA SEÑAL DE VIDA
//PB5 RESET


void adc_setup(void) {
    // Set the ADC input to PB2/ADC1
    ADMUX |= (1 << MUX0);
    ADMUX |= (1 << ADLAR);
    // Set the prescaler to clock/128 & enable ADC
    ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (0 << ADPS0) | (1 << ADEN);
}

int adc_read(void) {
    // Start the conversion
    ADCSRA |= (1 << ADSC);
    // Wait for it to finish 
    while (ADCSRA & (1 << ADSC));
    return ADCH;
}

long time_alive = TIME_PULSOS;
long time_read_pin = TIME_PWM;
unsigned short int timeout_off = TIME;
unsigned long int ignition_on = 0;
unsigned long int ignition_off = 0;
unsigned short int read = 0;
unsigned short int in_ignition = 0;
unsigned short int pin_vida = 0;// SEÑAL DE VIDA, PERRO GUARDIAR QUE ASEGURA QUE EL CORE SE ENCUENTRA SIEMPRE FUNCIONANDO
unsigned short int pin_vida_old = 0;// SEÑAL DE VIDA, PERRO GUARDIAR QUE ASEGURA QUE EL CORE SE ENCUENTRA SIEMPRE FUNCIONANDO
unsigned short int counter_flag = 5000;


int main(void) {

    ignition_on = 0;
    PORTB = 0x00;
    DDRB = 0x0A; // SALIDAS PORTB3 (LED) Y PORTB1 CONMUTACION VOLTAJE
    adc_setup();
	  
    while (1) //CICLO INFINITO DE PROGRAMA
    {
        in_ignition = (PINB & 0X01);

        while (adc_read() < 10 || (in_ignition == 1)) // VOLTAJE SUPERIOR A REFERENCIA Y IGNICION ACTIVA PARA SEGUIR //32 //17
        {
          _delay_ms(300);
		  in_ignition = (PINB & 0X01);
		  PORTB &= ~(1 << VOLT_OUT);
	    }

        ignition_on = TIME;    //UN MINUTOS
        ignition_off = REST;  //5 SEGUNDOS
        timeout_off = TIME;   // 5 SEGUNDOS

		while (timeout_off >= 1)
		{		

	if (!PORTB & (1 << VOLT_OUT))
		{
			counter_flag = 0x00;
			time_alive = TIME_PULSOS;
			time_read_pin = TIME_PWM;
		}
			time_alive--;
			time_read_pin--;
			if (time_read_pin == 0)
			{
				pin_vida_old = pin_vida;
				pin_vida = (PINB & 0X10);
				time_read_pin = TIME_PWM;
				if ((pin_vida_old == 0x00) && (pin_vida == 0x10))
					{
						counter_flag++;
					}
			}
		
			if (time_alive == 0)
			{
				time_alive = TIME_PULSOS;
				if (counter_flag > 7000)// definir cual es el tope de pulsos
				{
					counter_flag = 0;
					PORTB |= (1 << VOLT_OUT);
				}
				else
				{
				PORTB &= ~(1 << VOLT_OUT);
				_delay_ms(12000);
				if (PORTB & (1 << LED_IGN)) 
				{
				PORTB |= (1 << VOLT_OUT);
				}
				counter_flag = 0x00;
				}
			}		
					
            read = adc_read(); // ACTUALIZA NUEVO VALOR DE LECTURA
						
            if (read > 10)  // SI EL ESTADO ACTUAL Y EL ANTERIOR SON MAYORES A LA REFERENCIA ACTUALIZA TIMEOUT //32//17
            {
                in_ignition = (PINB & 0X01);
		
                if (in_ignition == 1) // SI IGNITION ESTA ACTIVA VERIFICA QUE PERMANEZCA ACTIVA POR 5 SEGUNDOS 
                {
                    ignition_on = TIME; //BORRAR
					if (ignition_off < 5355000)
                    {
						PORTB &= ~(1 << LED_IGN);
                    }
					if (ignition_off == 0) 
                    {
                        timeout_off = TIME;
						PORTB &= ~(1 << VOLT_OUT);
                    }
                    else 
                    {
						ignition_off--;
                    }
                }
                else 
                {
					ignition_off = REST;
					if (ignition_on == 0)
                    {
						PORTB |= (1 << LED_IGN);
						PORTB |= (1 << VOLT_OUT);
						//_delay_ms(50000);				
                    }
                    else
                    {
						ignition_on--;
                    }
                }
            }
            else
            {
                timeout_off--;
            }
        }
        PORTB &= ~(1 << LED_IGN);
        PORTB &= ~(1 << VOLT_OUT);
    }

}