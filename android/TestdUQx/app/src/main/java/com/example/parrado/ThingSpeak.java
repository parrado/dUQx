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

import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.net.HttpURLConnection;
import java.net.URL;

public class ThingSpeak {
	
	//Identificador del canal a usar
    //private static final String THINGSPEAK_CHANNEL_ID = "REPLACE_WITH_YOUR_CHANNEL_ID";
    private int channel_id;

    //Claves del canal
    //private static final String THINGSPEAK_WRITE_API_KEY = "REPLACE_WITH_YOUR_API_KEY";
    //private static final String THINGSPEAK_READ_API_KEY = "REPLACE_WITH_YOUR_API_KEY";
    //(Write API Key)
    private String write_key;
    //(Read API Key)
    private String read_key;


    private static final String THINGSPEAK_API_KEY_STRING = "api_key";
    
    private int nfields;



    //URL para actualizar el canal, enviar información a la nube
    private static final String THINGSPEAK_UPDATE_URL = "https://api.thingspeak.com/update?";

    //URL para leer los datos del canal, recibir información de la nube
    private static final String THINGSPEAK_CHANNEL_URL = "https://api.thingspeak.com/channels/";

    //para recibir la última entrada a un canal
    private static final String THINGSPEAK_FEEDS_LAST = "/feeds/last?";


    public void setChannel_id(int achannel_id){
        channel_id=achannel_id;
    }

    public void setWrite_key(String awrite_key){
        write_key=awrite_key;
    }
    
    
    public void setRead_key(String aread_key){


    	read_key=aread_key;
    }

    public boolean writeData(Double f[], Integer fields[]){

        if(f.length != fields.length)
            return  false;

        String url_str=new String();

        url_str=THINGSPEAK_UPDATE_URL +  THINGSPEAK_API_KEY_STRING + "=" +
                write_key;

        for(int i=1;i<=f.length;i++)
            url_str=url_str+"&field"+fields[i-1]+"="+f[i-1];

        try {


            //Crea la URL que se requiere para enviar datos al servidor ThingSpeak, en este caso la
            //latitud y la longitud que va alos campos field1 y field2 respectivamente
            URL url = new URL(url_str);

            //Se establece la conexión con el servidor. En la URL se envían los datos
            HttpURLConnection urlConnection = (HttpURLConnection) url.openConnection();
            try {
                //Estas líneas solo se utilizan para leer la respuesta del servidor
                BufferedReader bufferedReader = new BufferedReader(new InputStreamReader(urlConnection.getInputStream()));
                StringBuilder stringBuilder = new StringBuilder();
                String line;
                while ((line = bufferedReader.readLine()) != null) {
                    stringBuilder.append(line).append("\n");
                }
                bufferedReader.close();
                return  true;
            }
            finally{

                if(urlConnection.getInputStream()==null) {
                    urlConnection.disconnect();
                    return false;
                }
                else
                {
                    urlConnection.disconnect();
                    return true;
                }



            }
        }
        catch(Exception e) {

            return  false;


        }


    }
    
     public boolean writeData(Double f[]){

         Integer fields[];

         fields=new Integer[f.length];

         for(int i=0;i<f.length;i++)
             fields[i]=i+1;


         return writeData(f, fields);

    }



     public String readData(){
    	 
    	 String response=null;
    	
    	try {
               //La URL se forma para leer el canal
               URL url = new URL(THINGSPEAK_CHANNEL_URL + channel_id +
                       THINGSPEAK_FEEDS_LAST + THINGSPEAK_API_KEY_STRING + "=" + read_key);

               //Se establece la conexión
               HttpURLConnection urlConnection = (HttpURLConnection) url.openConnection();

               try {
                   //En este caso si importa la respuesta del servidor pues allí viene la información de localización registrada
                   //en ThingSpeak
                   BufferedReader bufferedReader = new BufferedReader(new InputStreamReader(urlConnection.getInputStream()));
                   StringBuilder stringBuilder = new StringBuilder();
                   String line;
                   while ((line = bufferedReader.readLine()) != null) {
                       stringBuilder.append(line).append("\n");
                   }
                   bufferedReader.close();
                   response = stringBuilder.toString();
                   
                   

                  
               } finally {
                   urlConnection.disconnect();
               }
           } catch (Exception e) {
              
               
           }
   //
    	 
    	return response;
        
   
    }
    	

}
