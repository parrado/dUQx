/*
 * dUQx firmware , ADC functions
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






void ADCRefSet(uint8_t ref)
{
	ADMUX = (ADMUX & ADC_REF_MASK) | (~ADC_REF_MASK & ref);
}

void ADCInit()
{

ADCRefSet(ADC_REF_AVCC);

ADCSRA &= (~(1 << ADATE)) & (~(1 << ADIE));

	

  ADCPrescalerSet(ADC_PRESCLR_128); // ADC clock = ~130 KHz (with 16.5 MHz clock)

DIDR0=0x3f;

ADCSRB = 0;

ADCSRA |= (1 << ADEN);


}

void ADCPrescalerSet(uint8_t pres)
{
	ADCSRA = (ADCSRA & ADC_PRESCLR_MASK) | (~ADC_PRESCLR_MASK & pres);
}

void ADCEnable()
{
	ADCSRA |= (1 << ADEN);
}

void ADCDisable()
{
	ADCSRA &= ~(1 << ADEN);
}

uint16_t ADCReadSingle(uint8_t ch)
{

uint8_t admux;

admux=ADMUX;


ADMUX = (ADMUX & 0xF0) | ((ch & 0x0f) );

if((admux&0x0f)!=(ch&0x0f))
	_delay_us(150);

ADCSRA |= (1 << ADSC);

while(ADCSRA & (1<<ADSC));


return ADC;
}




