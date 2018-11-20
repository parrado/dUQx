/*
 * dUQx C test file 1
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include <stdint.h>
#include "dUQx.h"






int main(int argc, char **argv)
{

uint8_t ch;

uint16_t pin;

uint16_t nsamples,i;

double *samples;

double v,vref;

//Inits dUQx board
//dUQx_Init(10); //10->For COM11 in Windows
dUQx_Init(24); //24 -> For /dev/ttyACM0 in linux

//Sets 10-bit resolution
dUQX_SetResolution(1);

//Digital port is configured as output
dUQx_ConfigureDigital(0x3ff);

//Writes 289h number on digital port
dUQx_WriteDigital(0x289);

//Reads digital port
dUQx_ReadDigital(&pin);

//Estimates reference voltage
dUQx_CalibrateAnalog(&vref);

ch=14;
//Reads input analog channel 14
dUQx_ReadAnalogSingle(ch,vref,&v);

//Writes 0.67 volts on output analog channel 0 (PWM)
dUQx_WriteAnalog(0.67,vref,0);

//Writes 2.7 volts on output analog channel 1 (PWM)
dUQx_WriteAnalog(2.7,vref,1);



//Prints results
printf("******* dUQx test program 1 *******\n\n");
printf("Digital reading: %x\n",pin);
printf("Vref: volts %f\n",vref);
printf("Vch%d: %f volts\n",ch,v);

//Samples to be read
nsamples=100;

//Reads 100 samples using block mode
nsamples=dUQx_ReadAnalogBuffer(ch,vref,&samples,nsamples);

//Prints results
printf("\n\nBlock mode acquired samples\n\n");
for(i=0;i<nsamples;i++)
printf("%f volts\n",samples[i]);

//Reads 100 samples using single sample mode
printf("\n\nSingle sample mode acquired samples\n\n");
for(i=0;i<nsamples;i++){

dUQx_ReadAnalogSingle(ch,vref,&v);

printf("%f volts\n",v);
}


//Closes dUQx
dUQx_End();




    return 0;
}
