/*
 * dUQx firmware , USART file
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
#include "serial.h" 

#include <stdint.h>


void write_byte(uint8_t b){

  // wait until the port is ready to be written to
  while( ( UCSR0A & ( 1 << UDRE0 ) ) == 0 ){} 
  // write the byte to the serial port
  UDR0 = b;
}


uint8_t read_byte(void){

uint8_t received_byte;

 // wait until a byte is ready to read
  while( ( UCSR0A & ( 1 << RXC0 ) ) == 0 ){}
 
  // grab the byte from the serial port
  received_byte = UDR0;

   return received_byte;

}


void init_serial(uint32_t br){

uint16_t prescale;



prescale=207;

switch(br){
case 2400:

prescale=832;
break;

case 4800:

prescale=416;
break;

case 9600:

prescale=207;
break;

case 14400:

prescale=138;
break;

case 19200:

prescale=103;
break;

case 28800:

prescale=68;
break;

case 38400:

prescale=51;
break;

case 57600:

prescale=34;
break;


case 76800:

prescale=25;
break;


case 115200:

prescale=16;
break;


case 230400:
prescale=8;
break;


case 250000:
prescale=7;
break;


case 500000:
prescale=3;
break;


case 1000000:
prescale=1;
break;


}



 UBRR0H  = (prescale >> 8);
 UBRR0L  = prescale;

 UCSR0B |= (1<<RXEN0)  | (1<<TXEN0);
 UCSR0C |= (1<<UCSZ00) | (1<<UCSZ01);

 UCSR0A |=(1<<1);

}



