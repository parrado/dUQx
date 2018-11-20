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



#ifndef DUQX_H_
#define DUQX_H_

#include <stdint.h>

//Definir para usar con módulo bluetooth HC06 y  aplicación en Android, para USB comentar esta línea
//#define BLUETOOTH

/*Inicia dUQx*/
uint8_t dUQx_Init(void);

void dUQx_End(void);

/*Verifica si dUQx está conectada*/
void dUQx_IsReady(void);

/*Configura el puerto digital*/
void dUQx_ConfigureDigital(uint16_t );

/*Escribe el puerto digital*/
void dUQx_WriteDigital(uint16_t );

/*Lee el puerto digital*/
void dUQx_ReadDigital(uint16_t *);

/*Escribe el canal análogo*/
void dUQx_WriteAnalog(double,double);

/*Lee un canal análogo*/
void dUQx_ReadAnalogSingle(uint8_t ch,double vref,double * v);

/*Calibra el controlador, retorna el voltaje de referencia estimado*/
void dUQx_CalibrateAnalog(double *);

/*Retorna la cantidad de muestras listas en el buffer*/
void dUQx_ADCBufferSize(uint8_t *size);

/*Selecciona entre la resolución de 10 bits (r=1) y 8 bits (r=0)*/
/*Por defecto dUQx opera a una resolución de 10 bits*/
void dUQX_SetResolution(uint8_t r);

/*Solicita un bloque de n datos del canal ch, v queda apuntando al bloque de datos*/
/*Retorna la cantidad de muestras capturadas*/
uint8_t dUQx_ReadAnalogBuffer(uint8_t ch,double vref,double ** v,uint8_t n);

/*Preescalador del reloj del ADC*/
void dUQx_ADCPreescalerSet(uint8_t ps);

/*Configura el temporizador que controla la tasa de muestreo en el modo bloque*/
void dUQx_ADCSampleRateSet(uint8_t sr);

/*Habilita (e=1) o inhabilita (e=0) el ADC*/
void dUQx_ADCEnabledSet(uint8_t e);

/*Se borra el FIFO de captura*/
void dUQx_ADCBufferClear(void);

#endif /*DUQX_H_*/
