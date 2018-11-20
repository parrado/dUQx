/*
 * dUQx C test file 3
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


#include <time.h>
#include <pthread.h>


#define BILLION 1e9

#ifdef _WIN32
#include <Windows.h>
double get_wall_time(){
    LARGE_INTEGER time,freq;
    if (!QueryPerformanceFrequency(&freq)){
        //  Handle error
        return 0;
    }
    if (!QueryPerformanceCounter(&time)){
        //  Handle error
        return 0;
    }
    return (double)time.QuadPart / freq.QuadPart;
}

#endif // _WIN32


int main(int argc, char **argv)
{


uint16_t pin;

uint16_t nsamples,i;



double t_elapsed;

#if defined(__linux__) || defined(__FreeBSD__)
struct timespec  start, stop;
#endif // defined

double *samples;

double v,vref;


//24 -> /dev/ttyACM0 in linux
//10 -> COM11
//dUQx_Init(10);
dUQx_Init(24);






dUQX_SetResolution(1);



dUQx_ConfigureDigital(0x3ff);


dUQx_WriteDigital(0x289);


dUQx_ReadDigital(&pin);

dUQx_CalibrateAnalog(&vref);

printf("%f\n",vref);



nsamples=1000;






dUQx_ReadAnalogSingle(14,vref,&v);
dUQx_WriteAnalog(0.67,vref,0);
dUQx_WriteAnalog(1.33,vref,1);

printf("Digital reading: %x\n",pin);
printf("Vref: %f\n",vref);
printf("Vch0: %f\n",v);


nsamples=1000;




#if defined(__linux__) || defined(__FreeBSD__)
clock_gettime( CLOCK_REALTIME, &start);
#endif // defined

#ifdef _WIN32
t_elapsed=get_wall_time();
#endif // _WIN32

nsamples=dUQx_ReadAnalogBuffer(14,vref,&samples,nsamples);
#if defined(__linux__) || defined(__FreeBSD__)
clock_gettime( CLOCK_REALTIME, &stop);
t_elapsed = ( stop.tv_sec - start.tv_sec )+ (double)( stop.tv_nsec - start.tv_nsec )/BILLION;
#endif // defined

#ifdef _WIN32
t_elapsed=get_wall_time()-t_elapsed;
#endif // _WIN32



printf( "Average sampling rate for %d samples is %lf Hz (buffer read)\n",nsamples,(double)nsamples/t_elapsed );

//for(i=0;i<nsamples;i++)
//printf("%f\n",samples[i]);

nsamples=1000;

samples=(double *)malloc(nsamples*sizeof(double));

#if defined(__linux__) || defined(__FreeBSD__)
clock_gettime( CLOCK_REALTIME, &start);
#endif // defined
#ifdef _WIN32
t_elapsed=get_wall_time();
#endif // _WIN32

for(i=0;i<nsamples;i++){

dUQx_ReadAnalogSingle(14,vref,&samples[i]);






}



#if defined(__linux__) || defined(__FreeBSD__)
clock_gettime( CLOCK_REALTIME, &stop);
t_elapsed = ( stop.tv_sec - start.tv_sec )+ (double)( stop.tv_nsec - start.tv_nsec )/BILLION;
#endif

#ifdef _WIN32
t_elapsed=get_wall_time()-t_elapsed;
#endif // _WIN32














printf( "Average sampling rate for %d samples is %lf Hz (single read)\n",nsamples,(double)nsamples/t_elapsed );

dUQx_End();

free(samples);


    return 0;
}
