/*
 * dUQx firmware , main application
 *
 * This file is part of dUQx.
 *
 * Copyright (c) 2018 Alexander López Parrado
 *
 * dUQx is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * dUQx is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with dUQx.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "adc.h"
#include "duqx.h"
#include "serial.h"


#include "requests.h"

#define BTTIMEOUT 500000
#define BTBRATE 57600
#define ATBRATE "AT+BAUD7"


void iniciar_timer(void);
void configure_digital(uint16_t  );

void write_digital(uint16_t );
uint16_t read_digital(void);

void escribir_dac(uint16_t );
uint16_t leer_adc(uint8_t );



uint8_t adc_10bit_mode=1;



uint8_t processcommand(uint8_t command)
{

    uint16_t reading,nsamples,i;

    uint8_t buffer[3],ch;


    switch(command)
    {

    case DUQX_ADC_READ_SINGLE:

        reading = ADCReadSingle(read_byte());


        if(adc_10bit_mode)
        {
            write_byte((uint8_t)(reading&0xff));
            write_byte((uint8_t)(reading>>8));
            return 2;


        }

        else
        {
            reading=reading>>2;
            write_byte((uint8_t)(reading&0xff));

            return 1;
        }


    case DUQX_ADC_READ_BUFFER:

        ch = (read_byte());
        buffer[0]=read_byte();
        buffer[1]=read_byte();

        nsamples=((uint16_t)(buffer[1])<<8)| (uint16_t)(buffer[0]);

        for(i=0; i<nsamples; i++)
        {

            reading = ADCReadSingle(ch);


            if(adc_10bit_mode)
            {
                write_byte((uint8_t)(reading&0xff));
                write_byte((uint8_t)(reading>>8));



            }

            else
            {
                reading=reading>>2;
                write_byte((uint8_t)(reading&0xff));


            }
        }


        return nsamples;


    case DUQX_ADC_PRESCALER_SET:
        ADCPrescalerSet(read_byte());

        return 0;



    case DUQX_ADC_ENABLED_SET:
        if(read_byte())
            ADCEnable();
        else
            ADCDisable();

        return 0;



    case DUQX_ADC_RESOLUTION_SET:
        adc_10bit_mode = read_byte();


        return 0;

    case DUQX_DAC_WRITE_SINGLE:

        buffer[0]=read_byte();
        buffer[1]=read_byte();
        buffer[2]=read_byte();

        if((buffer[2]&0x01)==0)

            escribir_dac_ch0((uint16_t)(buffer[1])<<8| (uint16_t)(buffer[0]));

        else

            escribir_dac_ch1((uint16_t)(buffer[1])<<8| (uint16_t)(buffer[0]));

        return 0;

    case DUQX_DIGITAL_CONFIGURE:

        buffer[0]=read_byte();
        buffer[1]=read_byte();

        configure_digital((uint16_t)(buffer[1])<<8| (uint16_t)(buffer[0]));

        return 0;

    case DUQX_DIGITAL_READ:

        reading=read_digital();

        write_byte((uint8_t)(reading&0xff));
        write_byte((uint8_t)(reading>>8));
        return 2;

    case DUQX_DIGITAL_WRITE:
        buffer[0]=read_byte();
        buffer[1]=read_byte();
        write_digital((uint16_t)(buffer[1])<<8| (uint16_t)(buffer[0]));
        return 0;






    }

    return 0;
}



//Tests if HC-06 bluetooth module is connected
uint8_t is_hc06(char * response,uint8_t delay)
{

    uint64_t counter;
    uint8_t nchar;

    write_byte('A');
    write_byte('T');

    counter=0;
    nchar=0;



    while(counter<(delay*BTTIMEOUT))
    {

// wait until a byte is ready to read
        if( ( UCSR0A & ( 1 << RXC0 ) ) )
        {
            response[nchar]=UDR0;
            nchar++;
            if(nchar>=2)
            {
                if((response[0]=='O') && (response[1]=='K'))
                    return 1;
            }
        }
        counter++;
    }

    return 0;





}




char aux_str[100];

int main(void)
{

    uint8_t command,flag_hc06;
    uint32_t i;

    // 1. Set USART baud rate to 9600
    // 2. Check if HC-06 bluetooth module is connected through AT-command, if so set baud rate to 57600 for both USART and HC-06. End
    // 3. Set USART baud rate to 57600
    // 4. Check if HC-06 bluetooth module is connected through AT-command, if HC-06 is not connected then set USART baud rate to 500000.


    DDRB=(1<<5);


   




#ifdef BLUETOOTH


    init_serial(BTBRATE);
    flag_hc06=is_hc06(aux_str,1);


    if(flag_hc06)
    {

        PORTB=(1<<5);



    }
    else
    {
        init_serial(9600);
        flag_hc06=is_hc06(aux_str,6);


        if(flag_hc06)
        {
            for(i=0; i<8; i++)
                write_byte(ATBRATE[i]);



                while(1){
   PORTB = PORTB | (1<<5);
_delay_ms(100);
        PORTB = PORTB & (~(1<<5));     
_delay_ms(100);
          }
        }
        else
        {

            PORTB=0;
	while(1);
        }
    }

#else
    init_serial(500000);
PORTB=(1<<5);
#endif




    ADCInit();
    iniciar_timer();



    while(1)
    {
        command=read_byte();
        processcommand(command);
    }

    return 0;
}



/* ------------------------------------------------------------------------- */


void configure_digital(uint16_t ddr )
{



    if(ddr & 0x200)
        DDRB = DDRB | (1<<5);

    else
        DDRB = DDRB & (~(1<<5));


    if(ddr & 0x100)
        DDRB = DDRB | (1<<4);
    else
        DDRB = DDRB & (~(1<<4));



    if(ddr & 0x080)
        DDRB = DDRB | (1<<3);

    else
        DDRB = DDRB & (~(1<<3));





    if(ddr & 0x040)
        DDRB = DDRB | (1<<0);

    else
        DDRB = DDRB & (~(1<<0));





    if(ddr & 0x020)
        DDRD = DDRD | (1<<7);

    else
        DDRD = DDRD & (~(1<<7));



    if(ddr & 0x010)
        DDRD = DDRD | (1<<6);

    else
        DDRD = DDRD & (~(1<<6));


    if(ddr & 0x008)
        DDRD = DDRD | (1<<5);
    else
        DDRD = DDRD & (~(1<<5));


    if(ddr & 0x004)
        DDRD = DDRD | (1<<4);
    else
        DDRD = DDRD & (~(1<<4));



    if(ddr & 0x002)
        DDRD = DDRD | (1<<3);
    else
        DDRD = DDRD & (~(1<<3));

    if(ddr & 0x001)
        DDRD = DDRD | (1<<2);
    else
        DDRD = DDRD & (~(1<<2));

}


void write_digital(uint16_t port)
{


    if(port & 0x200)
        PORTB = PORTB | (1<<5);

    else
        PORTB = PORTB & (~(1<<5));


    if(port & 0x100)
        PORTB = PORTB | (1<<4);
    else
        PORTB = PORTB & (~(1<<4));



    if(port & 0x080)
        PORTB = PORTB | (1<<3);

    else
        PORTB = PORTB & (~(1<<3));





    if(port & 0x040)
        PORTB = PORTB | (1<<0);

    else
        PORTB = PORTB & (~(1<<0));





    if(port & 0x020)
        PORTD = PORTD | (1<<7);

    else
        PORTD = PORTD & (~(1<<7));



    if(port & 0x010)
        PORTD = PORTD | (1<<6);

    else
        PORTD = PORTD & (~(1<<6));


    if(port & 0x008)
        PORTD = PORTD | (1<<5);
    else
        PORTD = PORTD & (~(1<<5));


    if(port & 0x004)
        PORTD = PORTD | (1<<4);
    else
        PORTD = PORTD & (~(1<<4));



    if(port & 0x002)
        PORTD = PORTD | (1<<3);
    else
        PORTD = PORTD & (~(1<<3));

    if(port & 0x001)
        PORTD = PORTD | (1<<2);
    else
        PORTD = PORTD & (~(1<<2));

}






uint16_t read_digital(void)
{

    uint16_t aux;


    aux= ((PINB&(1<<5))<<4) | ((PINB&(1<<4))<<4) |
         ((PINB&(1<<3))<<4) | ((PINB&(1<<0))<<6) |
         ((PIND&(1<<7))>>2) | ((PIND&(1<<6))>>2) |
         ((PIND&(1<<5))>>2) | ((PIND&(1<<4))>>2) |
         ((PIND&(1<<3))>>2)  | ((PIND&(1<<2))>>2) ;


    return aux;


}


void iniciar_timer(void)
{

    uint8_t sreg;

    TCCR1A=0xA2;
    TCCR1B=0x19;


    sreg=SREG;
    cli();
    ICR1=1024;
    SREG=sreg;

    sreg=SREG;
    cli();
    OCR1A=0;
    OCR1B=0;
    SREG=sreg;

    DDRB |= (1<<2) | (1<<1);
}


void escribir_dac_ch0(uint16_t v)
{

    uint8_t sreg;

    v=v&0x3ff;

    sreg=SREG;
    cli();
    OCR1A=v;
    SREG=sreg;

}

void escribir_dac_ch1(uint16_t v)
{

    uint8_t sreg;

    v=v&0x3ff; 

    sreg=SREG;
    cli();
    OCR1B=v;
    SREG=sreg;

}
