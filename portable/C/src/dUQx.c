/*
 * dUQx C API file
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


/*Prototipos de la biblioteca pthread*/
#include <signal.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "RS-232/rs232.h"

#include "dUQx.h"




#include "requests.h"   /* custom request numbers */





#if defined(__linux__) || defined(__FreeBSD__)

#define MOD(a,b) ((((a)%(b))+(b))%(b))

struct plant_data{
 double h;
 double Td;
 double K;
 double tau;
};

typedef struct plant_data plant_data_t ;

plant_data_t temp_data,flow_data;

/*Atributos para el mutex*/
pthread_mutexattr_t mutex_attrib;

/*mutex para el display*/
pthread_mutex_t mutex_u_temp,mutex_y_temp,mutex_u_flow,mutex_y_flow;

double u_temp=0,y_temp=0,u_flow=0,y_flow=0;

/*tipo de dato para el tiempo*/
typedef  struct timespec mytime_t;

/*tipo de dato para los temporizadores*/
typedef struct itimerspec mytimer_t;

/*Tipos de dato hilo*/
pthread_t p_t0,p_t1;

#endif

/*Manejador del dispositivo*/
int handle;

/*Resolución, 1: 10 bits, 0: 8 bits*/
unsigned char res_mode=1;



 #if defined(__linux__) || defined(__FreeBSD__)

 /*Código de la tarea 1*/
void temp_task(plant_data_t *info)
{
/*Conjunto de señales que manejará*/
sigset_t set;
  /*Identificador del temporizador*/
timer_t timer_id;
  /*Número de la señal que generará el temporizador*/
int sig_num;
  /*Estructura con la configuración del evento del temporizador*/
    struct sigevent event;

 /*Estructura para almacenar el periodo del hilo*/
  mytime_t per;

  /*Estructura con la información temporal del temporizador*/
 mytimer_t timer_data;

 double yn;
 double yn1;

 double *u,un;

 int xins,Nd;



  /*Señal que será generada por el temporizador*/
  sig_num = SIGRTMAX-0;

  /*Se adiciona la señal*/
 sigemptyset (&set);
sigaddset (&set, sig_num);


/*Se configura el evento del temporizador*/
 event.sigev_notify = SIGEV_SIGNAL;
 event.sigev_signo = sig_num;

 /*Se crea el temporizador*/
 timer_create (CLOCK_REALTIME, &event, &timer_id);

/*Se configura el periodo del temporizador*/
  per.tv_sec=0;
  per.tv_nsec=(long)(info->h*1e9);



  timer_data.it_interval=per;
  timer_data.it_value=per;

  /*Se inicia el temporizador*/
 timer_settime (timer_id, 0, &timer_data, NULL);

 yn1=0;

 //Canditad de muestras que tendrá el buffer circular con que se emulará el retardo de transporte
 Nd=(long)(info->Td/info->h);

 u=(double *)malloc(Nd*sizeof(double));

 memset(u,0,Nd*sizeof(double));

 xins=0;

while (1) {

/*Se espera la señal del temporizador*/
sigwait(&set,&sig_num);



/*Se lee la señal de entrada a la planta*/
pthread_mutex_lock(&mutex_u_temp);
un=u_temp;
pthread_mutex_unlock(&mutex_u_temp);


u[xins]=un;
/*Método de euler para resolver la ecuación diferencial*/

yn=yn1+info->h*(info->K/info->tau*u[MOD((xins+Nd-1),Nd)]-(1.0/info->tau)*yn1);
yn1=yn;



/*Se escribe el voltaje de salida de la planta*/
pthread_mutex_lock(&mutex_y_temp);
y_temp=yn;
pthread_mutex_unlock(&mutex_y_temp);
xins=MOD((xins-1),Nd);
}
}

void flow_task(plant_data_t *info)
{
/*Conjunto de señales que manejará*/
sigset_t set;
  /*Identificador del temporizador*/
timer_t timer_id;
  /*Número de la señal que generará el temporizador*/
int sig_num;
  /*Estructura con la configuración del evento del temporizador*/
    struct sigevent event;

 /*Estructura para almacenar el periodo del hilo*/
  mytime_t per;

  /*Estructura con la información temporal del temporizador*/
 mytimer_t timer_data;

 double yn;
 double yn1;

 double *u,un;

 int xins,Nd;

  /*Señal que será generada por el temporizador*/
  sig_num = SIGRTMAX-1;

  /*Se adiciona la señal*/
 sigemptyset (&set);
sigaddset (&set, sig_num);


/*Se configura el evento del temporizador*/
 event.sigev_notify = SIGEV_SIGNAL;
 event.sigev_signo = sig_num;

 /*Se crea el temporizador*/
 timer_create (CLOCK_REALTIME, &event, &timer_id);

/*Se configura el periodo del temporizador*/
  per.tv_sec=0;
  per.tv_nsec=(long)(info->h*1e9);



  timer_data.it_interval=per;
  timer_data.it_value=per;

  /*Se inicia el temporizador*/
 timer_settime (timer_id, 0, &timer_data, NULL);

 yn1=0;

 Nd=(long)(info->Td/info->h);

 u=(double *)malloc(Nd*sizeof(double));

 memset(u,0,Nd*sizeof(double));

 xins=0;

while (1) {
        /*Se espera la señal del temporizador*/
sigwait(&set,&sig_num);

/*Método de euler para resolver la ecuación diferencial*/
pthread_mutex_lock(&mutex_u_flow);
un=u_flow;
pthread_mutex_unlock(&mutex_u_flow);

u[xins]=un;

yn=yn1+info->h*(info->K/info->tau*u[MOD((xins+Nd-1),Nd)]-1.0/info->tau*yn1);
yn1=yn;

pthread_mutex_lock(&mutex_y_flow);
y_flow=yn;
pthread_mutex_unlock(&mutex_y_flow);

		xins=MOD((xins-1),Nd);
}
}

#endif


/*Inicia dUQx*/
uint8_t dUQx_Init(int cport_nr) {


#if defined(__linux__) || defined(__FreeBSD__)



/*Conjunto de señales que manejará*/
sigset_t set;


#endif


int    bdrate=500000,ret;

  char mode[]={'8','N','1',0};

handle=cport_nr;

ret=RS232_OpenComport(cport_nr, bdrate, mode);



    //Waits for bootloader execution
    #ifdef _WIN32
    Sleep(3000);
#else
    sleep(3);  /* sleep for 1 Second */
#endif


    //10-bit mode by default
    res_mode=1;

    //Threads for plants simulation
    #if defined(__linux__) || defined(__FreeBSD__)

    //Parameters of temperature plant
    temp_data.h=10e-3;
    temp_data.K=0.8;
    temp_data.tau=40.0;
    temp_data.Td=11;

    //Parameters of flow plant
    flow_data.h=10e-3;
    flow_data.K=1.15;
    flow_data.tau=0.6424;
    flow_data.Td=0.075;


	/*Atributos por defecto del mutex*/
	pthread_mutexattr_init(&mutex_attrib);

	/*Inicializa el mutex*/
	pthread_mutex_init(&mutex_u_temp, &mutex_attrib);
	pthread_mutex_init(&mutex_u_flow, &mutex_attrib);
	pthread_mutex_init(&mutex_y_temp, &mutex_attrib);
	pthread_mutex_init(&mutex_y_flow, &mutex_attrib);





	sigemptyset (&set);
sigaddset (&set, SIGRTMAX-0);
sigaddset (&set, SIGRTMAX-1);
/*Se bloquean ambas señales ya que no habra un callback en el manejador,
* estas serán manejadas por sigwait()*/
 pthread_sigmask (SIG_BLOCK, &set, NULL) ;

 /*Se crean los dos hilos de emulación de las plantas*/
 pthread_create(&p_t0, NULL, (void *)temp_task, &temp_data);
 pthread_create(&p_t1, NULL, (void *)flow_task, &flow_data);

#endif



return(ret);

}

int readbytes(int mhandle, unsigned char *buffer, int size){

 int len = size;
        int recvbytes=0;
unsigned char * aux_ptr;

aux_ptr=buffer;

        //Mientras no se hayan leído todos los bytes (longitud de buffer)
        while(len>0){
            recvbytes=RS232_PollComport(mhandle, aux_ptr, len);
            aux_ptr += recvbytes;
            len-=recvbytes;

        }

        return (size-len);




}


void dUQx_End(void){

  RS232_CloseComport(handle);
    }

void dUQx_ConfigureDigital(uint16_t ddr){

uint8_t buffer[3];

buffer[0]=DUQX_DIGITAL_CONFIGURE;
buffer[1]=ddr&0xff;
buffer[2]=(ddr>>8)&0x3;

/*Writes commands to dUQx*/
RS232_SendBuf(handle, buffer, 3);







}


void dUQx_WriteDigital(uint16_t port){

uint8_t buffer[3];

/*Escribir digital*/

buffer[0]=DUQX_DIGITAL_WRITE;
buffer[1]=port&0xff;
buffer[2]=(port>>8)&0x3;

/*Writes commands to dUQx*/
RS232_SendBuf(handle, buffer, 3);




}


void dUQx_ReadDigital(uint16_t *pin)
{

uint8_t buffer[2];

buffer[0]=DUQX_DIGITAL_READ;

/*Writes commands to dUQx*/
RS232_SendBuf(handle, buffer, 1);

 /*Reads response from dUQx*/
readbytes(handle, buffer, 2);


*pin=((uint16_t)(buffer[1])<<8|(uint16_t)(buffer[0]))&0x3ff;

}


void dUQx_WriteAnalog(double v,double vref,uint8_t ch){

    uint16_t v_i;
    uint8_t buffer[4];







	if(v>vref) v=vref;
	if(v<0) v=0.0;

	if(ch<DAC_CHANNEL_TEMP){


	v_i= (uint16_t)(1024.0*(double)(v)/vref);

	 buffer[0]=DUQX_DAC_WRITE_SINGLE;
buffer[1]=v_i&0xff;
buffer[2]=(v_i>>8)&0x3;
buffer[3]=(ch)&0x01;

/*Writes commands to dUQx*/
RS232_SendBuf(handle, buffer, 4);

}else{

if(ch==DAC_CHANNEL_TEMP){
#if defined(__linux__) || defined(__FreeBSD__)
pthread_mutex_lock(&mutex_u_temp);
u_temp=v;
pthread_mutex_unlock(&mutex_u_temp);
#endif
}

if(ch==DAC_CHANNEL_FLOW){
#if defined(__linux__) || defined(__FreeBSD__)
pthread_mutex_lock(&mutex_u_flow);
u_flow=v;
pthread_mutex_unlock(&mutex_u_flow);
#endif
}




}



}



void dUQx_ReadAnalogSingle(uint8_t ch,double vref,double * v){


    uint16_t v_i;
    uint8_t buffer[2];

    buffer[0]=DUQX_ADC_READ_SINGLE;
    buffer[1]=ch&0x0f;

    if(ch<ADC_CHANNEL_TEMP){

    /*Writes commands to dUQx*/
RS232_SendBuf(handle, buffer, 2);





    if(res_mode){



     /*Reads response from dUQx*/
readbytes(handle, buffer, 2);


             v_i=((uint16_t)(buffer[1])<<8|(uint16_t)(buffer[0]))&0x3ff;

         *v=(((double)(v_i)*vref)/1024.0);

    }
     else{

     readbytes(handle, buffer, 1);;
             v_i=((uint16_t)(buffer[0]))&0xff;

         *v=(((double)(v_i)*vref)/256.0);

     }

}else{

if(ch==ADC_CHANNEL_TEMP){
#if defined(__linux__) || defined(__FreeBSD__)
pthread_mutex_lock(&mutex_y_temp);
*v=y_temp;
pthread_mutex_unlock(&mutex_y_temp);
#endif
}

if(ch==ADC_CHANNEL_FLOW){
#if defined(__linux__) || defined(__FreeBSD__)
pthread_mutex_lock(&mutex_y_flow);
*v=y_flow;
pthread_mutex_unlock(&mutex_y_flow);
#endif
}




}

}




uint16_t dUQx_ReadAnalogBuffer(uint8_t ch,double vref,double
** v,uint16_t n){

 uint16_t v_i,i,len;
    uint8_t *buffer;

    double *p_v;

    uint8_t buffer2[4];





/*Escribir digital*/

buffer2[0]=DUQX_ADC_READ_BUFFER;
buffer2[1]=ch;
buffer2[2]=n;
buffer2[3]=(n>>8);

RS232_SendBuf(handle, buffer2, 4);







    if(res_mode){


          p_v=(double*)malloc(n*sizeof(double));

    *v=p_v;

            buffer=(uint8_t*)malloc(2*n*sizeof(uint8_t));


            len=readbytes(handle, buffer, 2*n);
            //len=RS232_PollComport(handle, buffer, 2*n);



     len=len>>1;

for(i=0;i<len;i++){
 v_i=(((uint16_t)(buffer[2*i+1])<<8)|((uint16_t)(buffer[2*i])))&0x3ff;


         p_v[i]=(((double)(v_i)*vref)/1023.0);
}



    }
     else{


          p_v=(double*)malloc(n*sizeof(double));

    *v=p_v;

             buffer=(uint8_t*)malloc(n*sizeof(uint8_t));


         len=readbytes(handle, buffer, n);
         for(i=0;i<len;i++)
            p_v[i]=(((double)(buffer[i])*vref)/255.0);



     }


free(buffer);

     return len;



}


void dUQx_CalibrateAnalog(double *vref){

double aux;

	/*Se lee el voltaje del regulador*/
	dUQx_ReadAnalogSingle(14,1.0,&aux);

	/*Se lee el voltaje del regulador*/
	dUQx_ReadAnalogSingle(14,1.0,&aux);



	/*Se estima el voltaje de referencia*/
	aux=1.1/aux;

	*vref=aux;

}


void dUQX_SetResolution(uint8_t r){
    uint8_t buffer[2];

    res_mode=r;

    buffer[0]=DUQX_ADC_RESOLUTION_SET;
    buffer[1]=r;


   /*Writes commands to dUQx*/
RS232_SendBuf(handle, buffer, 2);




}






void dUQx_ADCPreescalerSet(uint8_t ps)
{
    uint8_t buffer[2];
           /*Leer digital*/
    buffer[0] =DUQX_ADC_PRESCALER_SET;
    buffer[1]=ps;

       /*Writes commands to dUQx*/
RS232_SendBuf(handle, buffer, 2);



}



void dUQx_ADCEnabledSet(uint8_t e)
{
        uint8_t buffer[2];
           /*Leer digital*/

      buffer[0] =DUQX_ADC_ENABLED_SET;
      buffer[1]=e&255;

   /*Writes commands to dUQx*/
RS232_SendBuf(handle,buffer, 2);





}






