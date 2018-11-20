package com.example.parrado;

import com.fazecast.jSerialComm.*;
import com.example.parrado.dUQx;

public class TestdUQx {

	public static void main(String[] args) {
		// TODO Auto-generated method stub
		SerialPort comPort = SerialPort.getCommPorts()[0];
		comPort.openPort();
		
		comPort.setComPortParameters(500000, 8, 0, 0);
		
		comPort.setComPortTimeouts(SerialPort.TIMEOUT_READ_BLOCKING, 0, 0);
		
		try {
			Thread.sleep(3000);
		} catch (InterruptedException e1) {
			// TODO Auto-generated catch block
			e1.printStackTrace();
		}
		
		dUQx mydUQx=new dUQx(comPort.getInputStream(),comPort.getOutputStream()); 
		
		
		System.out.println(mydUQx.dUQx_CalibrateAnalog());
		
		mydUQx.dUQx_WriteDigital(1023);
		System.out.println(mydUQx.dUQx_ReadDigital());
		
		mydUQx.dUQX_SetResolution((byte)1);
		
		double buffer[];
		
		int Ns=1000;
		
		double v;
		
		
		double start=(double)System.nanoTime();
		for(int i=0;i<Ns;i++)
		v=mydUQx.dUQx_ReadAnalogSingle(14);		
		double stop=(double)System.nanoTime();
		
		double sr=Ns/(stop-start);
		
		System.out.println("dUQx achieved a sample rate of " +String.valueOf(sr*1e6)+ " kHz when reading "+Ns+" samples in single sample mode.");
		
		
		start=(double)System.nanoTime();
		buffer=mydUQx.dUQx_ReadAnalogBuffer(14, Ns);
		stop=(double)System.nanoTime();
		
		sr=Ns/(stop-start);
		
		System.out.println("dUQx achieved a sample rate of " +String.valueOf(sr*1e6)+ " kHz when reading "+Ns+" samples in buffer mode.");
		
		
		comPort.closePort();

	}

}
