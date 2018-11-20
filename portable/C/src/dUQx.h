/*
 * dUQx C API file
 *
 * This file is part of dUQx.
 *
 * Copyright (c) 2018 Alexander L�pez Parrado
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

#ifndef DUQX_H_
#define DUQX_H_

#include <stdint.h>

#define ADC_CHANNEL_TEMP            16
#define ADC_CHANNEL_FLOW            17
#define DAC_CHANNEL_TEMP            2
#define DAC_CHANNEL_FLOW            3

/*Inicia dUQx*/
uint8_t dUQx_Init(int);

void dUQx_End(void);

/*Verifica si dUQx est� conectada*/
void dUQx_IsReady(void);

/*Configura el puerto digital*/
void dUQx_ConfigureDigital(uint16_t );

/*Escribe el puerto digital*/
void dUQx_WriteDigital(uint16_t );

/*Lee el puerto digital*/
void dUQx_ReadDigital(uint16_t *);

/*Escribe el canal an�logo*/
void dUQx_WriteAnalog(double,double,uint8_t);

/*Lee un canal an�logo*/
void dUQx_ReadAnalogSingle(uint8_t ch,double vref,double * v);

uint16_t dUQx_ReadAnalogBuffer(uint8_t ch,double vref,double
** v,uint16_t n);

/*Calibra el controlador, retorna el voltaje de referencia estimado*/
void dUQx_CalibrateAnalog(double *);


/*Selecciona entre la resoluci�n de 10 bits (r=1) y 8 bits (r=0)*/
/*Por defecto dUQx opera a una resoluci�n de 10 bits*/
void dUQX_SetResolution(uint8_t r);


/*Preescalador del reloj del ADC*/
void dUQx_ADCPreescalerSet(uint8_t ps);


/*Habilita (e=1) o inhabilita (e=0) el ADC*/
void dUQx_ADCEnabledSet(uint8_t e);


#endif /*DUQX_H_*/
