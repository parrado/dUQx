 # dUQx Python, API file
 #
 # This file is part of dUQx.
 #
 # Copyright (c) 2018 Alexander Lopez Parrado
 #
 # dUQx is free software: you can redistribute it and/or modify
 # it under the terms of the GNU Lesser General Public License as published by
 # the Free Software Foundation, either version 3 of the License, or
 # (at your option) any later version.
 #
 # dUQx is distributed in the hope that it will be useful,
 # but WITHOUT ANY WARRANTY; without even the implied warranty of
 # MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 # GNU Lesser General Public License for more details.
 #
 # You should have received a copy of the GNU Lesser General Public License
 # along with dUQx.  If not, see <https://www.gnu.org/licenses/>.
 

#Required modules
import serial
import sys
import time
import array
import struct

#Constants for dUQx commands
DUQX_ADC_READ_SINGLE=1;
DUQX_ADC_READ_BUFFER=2;
DUQX_ADC_PRESCALER_SET=3;
DUQX_ADC_ENABLED_SET=4;
DUQX_ADC_RESOLUTION_SET=5;
DUQX_DAC_WRITE_SINGLE=6;
DUQX_DIGITAL_CONFIGURE=7;
DUQX_DIGITAL_WRITE=8;
DUQX_DIGITAL_READ=9;


#Required global variables
f=0;
resmode=1;


#Inits dUQx
#Alexander Lopez Parrado(2018)
def init(portn):
	global f;
	if(sys.platform=='win32'):
		ports=['COM1', 'COM2','COM3','COM4','COM5','COM6','COM7','COM8','COM9',  'COM10', 			'COM11', 'COM12','COM13', 'COM14', 'COM15', 'COM16'];

	if(sys.platform=='linux2'):
		ports=['/dev/ttyS0','/dev/ttyS1','/dev/ttyS2','/dev/ttyS3','/dev/ttyS4','/dev/ttyS5','/dev/ttyS6','/dev/ttyS7','/dev/ttyS8','/dev/ttyS9','/dev/ttyS10','/dev/ttyS11', '/dev/ttyS12','/dev/ttyS13','/dev/ttyS14','/dev/ttyS15','/dev/ttyUSB0','/dev/ttyUSB1','/dev/ttyUSB2','/dev/ttyUSB3','/dev/ttyUSB4','/dev/ttyUSB5', '/dev/ttyAMA0','/dev/ttyAMA1','/dev/ttyACM0','/dev/ttyACM1', '/dev/rfcomm0','/dev/rfcomm1','/dev/ircomm0','/dev/ircomm1','/dev/cuau0','/dev/cuau1','/dev/cuau2','/dev/cuau3', '/dev/cuaU0','/dev/cuaU1','/dev/cuaU2','/dev/cuaU3'];

	#Inits serial port
	f = serial.Serial(ports[portn],500000,serial.EIGHTBITS) ;
	#Waits for Arduino reset
	time.sleep(3);




#Enables (e=1) or disables (e=0)  ADC
#Alexander Lopez Parrado (2018)
def adc_enabled_set(e):

	#Prepares command
	buffer=str(bytearray([DUQX_ADC_ENABLED_SET, e&255]));
	
	#Writes command to serial port
	f.write(buffer)


#Sets ADC clock preescaler by using ps value
#
#ps == 0, ADC clock is 16e6/2 Hz
#ps == 1, ADC clock is 16e6/2 Hz
#ps == 2, ADC clock is 16e6/4 Hz
#ps == 3, ADC clock is 16e6/8 Hz
#ps == 4, ADC clock is 16e6/16 Hz
#ps == 5, ADC clock is 16e6/32 Hz
#ps == 5, ADC clock is 16e6/64 Hz
#ps == 7, ADC clock is 16e6/128 Hz
#
#Default value is ps=7
#
#Alexander Lopez Parrado (2018)
def adc_preescaler_set(ps):

	buffer=str(bytearray([DUQX_ADC_PRESCALER_SET, ps&255]));

	f.write(buffer)
	

#Configura el puerto digital usando como registro de direccion de datos a
#ddr. Un bit en 1 en ddr configura como salida el pin respectivo, de lo
#contrario lo configura como entrada.
#
#Alexander Lopez Parrado(2018)
def configure_digital(ddr):

	
	buffer=str(bytearray([DUQX_DIGITAL_CONFIGURE, ddr&255,(ddr>>8)&3]));

	f.write(buffer);


#Closes dUQx
def end():
	f.close();

#Intenta leer hasta n muestras del canal analogico ch usando vref como
#voltaje de referencia.
#
#n debe ser menor o igual a 250 para el modo de 8 bits por muestra.
#n debe ser menor o igual a 125 para el modo de 10 bits por muestra.
#
#Las muestras capturadas son retornadas en el vector v.
#
##Alexander Lopez Parrado(2018)
def read_analog_buffer(ch,vref,n):


	v=[];


	buffer=str(bytearray([DUQX_ADC_READ_BUFFER, ch, n&255, (n>>8)&255]))

	f.write(buffer)

	if(resmode==1):
		
		buffer=list(f.read(2*n))	
		for i in range(len(buffer)):
			buffer[i]= struct.unpack("<B",buffer[i])[0]	
			
    
		for i in range (len(buffer)/2):
			v_i=(((buffer[2*i+1])<<8)|((buffer[2*i])))&0x3ff
			v.append((v_i*vref)/1024.0)    
	else:

		buffer=list(f.read(n))	
		for i in range(len(buffer)):
			buffer[i]= struct.unpack("<B",buffer[i])[0]
		
		for i in range(len(buffer)):
			v.append(((buffer[i]&255)*vref)/256.0)

	return v


#Lee una unica muestra del canal analogico ch usando como voltaje de
#referencia a vref. El voltaje leido es retornado en v.
#
#Alexander Lopez Parrado(2018)
def read_analog_single(ch,vref):

	buffer=str(bytearray([DUQX_ADC_READ_SINGLE, ch]))

	f.write(buffer)

	if(resmode==1):

		buffer=list(f.read(2))
		buffer[0]= struct.unpack("<B", buffer[0])[0]
		buffer[1]= struct.unpack("<B", buffer[1])[0]			
    		
		v_i=(((buffer[1])<<8)|((buffer[0])))&0x3ff
		v=(v_i*vref)/1024.0
	else:

		buffer=list(f.read(1))	

		buffer[0]= struct.unpack("<B",buffer[0])[0]		

		v=((buffer[0]&255)*vref)/256.0;

	return v


#Retorna en vref el voltaje referencia para los canales analogos. 
#
#Alexander Lopez Parrado(2018)
def calibrate_analog():

	read_analog_single(14,1.0)
	return 1.1/read_analog_single(14,1.0)


#Lee el puerto digital y retorna su estado en pin
#
#Alexander Lopez Parrado(2018)
def read_digital():

	buffer=str(bytearray([DUQX_DIGITAL_READ]))
	f.write(buffer);

	buffer=list(f.read(2))
	buffer[0]= struct.unpack("<B", buffer[0])[0]
	buffer[1]= struct.unpack("<B", buffer[1])[0]	

	pin=((buffer[1])<<8|(buffer[0]))&0x3ff;
	return pin

#Fija el tamano de bits por muestra capturada en el canal analogico en 8 bits
#(r=0) o 10 bits (r=1).
#
#El tamano por defecto usado por dUQx es de 10 bits
#
#Alexander Lopez Parrado(2018)
def set_resolution(r):

	global resmode;

	resmode=r;

	buffer=str(bytearray([DUQX_ADC_RESOLUTION_SET, r&255]))
	f.write(buffer);

#Escribe el voltaje v (nivel de DC de la senal PWM) en el canal ch usando a vref como voltaje de
#referencia.
#Alexander Lopez Parrado(2018)
def write_analog(v,vref,ch):


	if(v>vref):
		v=vref


	if(v<0): 
		v=0.0	

	v_i= int(round((1024.0*(v)/vref)))

	buffer=str(bytearray([DUQX_DAC_WRITE_SINGLE, v_i&255, (v_i>>8)&3,ch&255]))

	f.write(buffer)


#Escribe el valor de port en el puerto digital
#Alexander Lopez Parrado(2018)
def write_digital(port):

	buffer=str(bytearray([DUQX_DIGITAL_WRITE, port&255,(port>>8)&3]));
	f.write(buffer);
	
