/*
 * dUQx Java API file
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

package com.example.parrado;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;


//Clase dUQx
public class dUQx  {

    //Listado de peticiones soportadas por dUQX
    private static final byte DUQX_ADC_READ_SINGLE = 1;
    private static final byte DUQX_ADC_READ_BUFFER = 2;
    private static final byte DUQX_ADC_PRESCALER_SET = 3;
    private static final byte DUQX_ADC_ENABLED_SET = 4;
    private static final byte DUQX_ADC_RESOLUTION_SET = 5;
    private static final byte DUQX_DAC_WRITE_SINGLE = 6;
    private static final byte DUQX_DIGITAL_CONFIGURE = 7;
    private static final byte DUQX_DIGITAL_WRITE = 8;
    private static final byte DUQX_DIGITAL_READ = 9;


    //Miembro que almacena la petición actual
    private byte request;

    //Miembro que almacena el voltaje de referencia
    private double vref;

    //Modo de resolución del ADC (res_mode = 0, 8 bits; res_mode = 1, 10 bits )
    private byte res_mode;

    //Puntos de conexión a dUQx para lectura y escritura
    private InputStream in;
    private OutputStream out;

    

    //Método para llenar buffer con bytes desde dUQX
    private void readbytes(byte buffer[]){

        int len = buffer.length;
        int recvbytes=0;

        //Mientras no se hayan leído todos los bytes (longitud de buffer)
        while(len>0){
            try {
                recvbytes=in.read(buffer,recvbytes,len);
                len-=recvbytes;

            } catch (IOException e) {
                e.printStackTrace();
            }

        }

    }








    //Constructor de dUQx
    public dUQx(InputStream ini, OutputStream outi) {
        in = ini;
        out = outi;
        request = -1;
        res_mode = 1;
        vref=0;
    }

    public void dUQx_ConfigureDigital(int ddr) {

        request = DUQX_DIGITAL_CONFIGURE;

        byte buffer[] = new byte[3];


        buffer[0] = request;
        buffer[1] = (byte) (ddr & 0xff);
        buffer[2] = (byte) ((ddr >> 8) & 0x3);

/*Writes commands to dUQx*/

        try {
            out.write(buffer);
        } catch (IOException e) {
            e.printStackTrace();
        }




    }


    public void dUQx_WriteDigital(int port) {

        request = DUQX_DIGITAL_WRITE;

        byte buffer[] = new byte[3];
        buffer[0] = request;

        /*Escribir digital*/
        buffer[0] = request;
        buffer[1] = (byte) (port & 0xff);
        buffer[2] = (byte) ((port >> 8) & 0x3);

        /*Writes commands to dUQx*/
        try {
            out.write(buffer);

        } catch (IOException e) {
            e.printStackTrace();
        }




    }


    public int dUQx_ReadDigital() {
        request = DUQX_DIGITAL_READ;

        byte buffer[] = new byte[2];

        int ret;


        buffer[0] = request;



/*Writes commands to dUQx*/
        try {
            out.write(buffer);
        } catch (IOException e) {
            e.printStackTrace();
        }



        readbytes(buffer);
        //Read digital
        ret = ((int) (buffer[1]<0?buffer[1]+256:buffer[1]) << 8 | (int) (buffer[0]<0?buffer[0]+256:buffer[0])) & 0x3ff;

        return ret;


    }


    public void dUQx_WriteAnalog(double v, int ch) {

        int v_i;

        request = DUQX_DAC_WRITE_SINGLE;

        byte buffer[] = new byte[4];


        if (v > vref) v = vref;
        if (v < 0) v = 0.0;


        v_i = (int) (1024.0 * (double) (v) / vref);

        buffer[0] = request;
        buffer[1] = (byte) (v_i & 0xff);
        buffer[2] = (byte) ((v_i >> 8) & 0x3);
        buffer[3] = (byte) ((ch) & 0x01);

/*Writes commands to dUQx*/
        try {
            out.write(buffer);

        } catch (IOException e) {
            e.printStackTrace();
        }




    }


    public double dUQx_ReadAnalogSingle(int chi){

        return dUQx_ReadAnalogSingle(chi,vref);

    }

    private double dUQx_ReadAnalogSingle(int ch,double vrefi) {


        int v_i;

        byte buffer[] = new byte[2];

        double ret;

        request = DUQX_ADC_READ_SINGLE;



        buffer[0] = request;
        buffer[1] = (byte) (ch & 0x0f);




/*Writes commands to dUQx*/
        try {
            out.write(buffer);
        } catch (IOException e) {
            e.printStackTrace();
        }



        if (res_mode == 1) {

            readbytes(buffer);

            v_i = ((int) (buffer[1]<0?buffer[1]+256:buffer[1]) << 8 | (int) (buffer[0]<0?buffer[0]+256:buffer[0])) & 0x3ff;

            ret = (((double) (v_i) * vrefi) / 1024.0);


        } else {

            buffer = new byte[1];

            readbytes(buffer);


            v_i = ((int) ((buffer[0]<0?buffer[0]+256:buffer[0]))) & 0xff;

            ret = (((double) (v_i) * vrefi) / 256.0);


        }


        return ret;




    }


    public double[] dUQx_ReadAnalogBuffer(int ch, int n) {

        int v_i, i, len;

        request=DUQX_ADC_READ_BUFFER;

        byte buffer[] = new byte[4];

        double ret[];


        buffer[0] = request;
        buffer[1] = (byte)ch;
        buffer[2] = (byte)n;
        buffer[3] = (byte) (n >> 8);

       /*Writes commands to dUQx*/
        try {
            out.write(buffer);
        } catch (IOException e) {
            e.printStackTrace();
        }

        if (res_mode==1) {


            ret = new double[n];

            buffer = new byte[2*n];


            readbytes(buffer);
            len = buffer.length >> 1;

            for (i = 0; i < len; i++) {

                v_i = (((int) ((buffer[2*i+1]<0?buffer[2*i+1]+256:buffer[2*i+1])) << 8) | ((int) (buffer[2*i]<0?buffer[2*i]+256:buffer[2*i]))) & 0x3ff;


                ret[i] = (((double) (v_i) * vref) / 1023.0);
            }


        } else {

            ret = new double[n];

            buffer = new byte[n];


            readbytes(buffer);

            len= buffer.length;


            for (i = 0; i < len; i++) {

                ret[i] = (((double) (((int)(buffer[i]<0?buffer[i]+256:buffer[i]))) * vref) / 255.0);
            }





        }


        return ret;


    }


    public double dUQx_CalibrateAnalog() {

        dUQx_ReadAnalogSingle((byte)14,1.0);
        vref=1.1/dUQx_ReadAnalogSingle((byte)14,1.0);
        return vref;



    }


    public void dUQX_SetResolution(byte r) {
        byte buffer[] = new byte[2];


        request = DUQX_ADC_RESOLUTION_SET;

        buffer[0] = request;

        res_mode = r;

        buffer[0] = request;
        buffer[1] = res_mode;


     /*Writes commands to dUQx*/
        try {
            out.write(buffer);

        } catch (IOException e) {
            e.printStackTrace();
        }




    }


    public void dUQx_ADCPreescalerSet(byte ps) {
        byte buffer[] = new byte[2];

        request = DUQX_ADC_PRESCALER_SET;
           /*Leer digital*/
        buffer[0] = request;
        buffer[1] = ps;

 /*Writes commands to dUQx*/
        try {
            out.write(buffer);

        } catch (IOException e) {
            e.printStackTrace();
        }




    }


    public void dUQx_ADCEnabledSet(byte eadc) {
        byte buffer[] = new byte[2];


        request = DUQX_ADC_ENABLED_SET;
        buffer[0] = request;
        buffer[1] = (byte) (eadc & 255);

   /*Writes commands to dUQx*/
        try {
            out.write(buffer);

        } catch (IOException e) {
            e.printStackTrace();
        }




    }




}
