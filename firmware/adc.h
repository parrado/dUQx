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


#define ADC_REF_MASK	~((1 << REFS1)|(1 << REFS0))

#define ADC_REF_AREF	(0 << REFS1)|(0 << REFS0)
#define ADC_REF_AVCC	(0 << REFS1)|(1 << REFS0)


#define ADC_PRESCLR_MASK	~((1 << ADPS2)|(1 << ADPS1)|(1 << ADPS0))
#define ADC_PRESCLR_2		0x01
#define ADC_PRESCLR_4		0x02
#define ADC_PRESCLR_8		0x03
#define ADC_PRESCLR_16		0x04
#define ADC_PRESCLR_32		0x05
#define ADC_PRESCLR_64		0x06
#define ADC_PRESCLR_128		0x07




void ADCRefSet(uint8_t ref);
void ADCInit();
void ADCEnable();
void ADCDisable();
uint16_t ADCReadSingle(uint8_t);
void ADCReadBuffer(uint8_t);
void ADCPrescalerSet(uint8_t pres);
ISR(ADC_vect);
void ADCSampleRateSet(uint8_t val);
