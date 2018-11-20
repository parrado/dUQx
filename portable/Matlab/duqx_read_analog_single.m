function v=duqx_read_analog_single(ch,vref)
%Lee una �nica muestra del canal analógicoo ch usando como voltaje de
%referencia a vref. El voltaje leído es retornado en v.
%
%Alexander L�pez Parrado(2017)

global duqxrequests;
global f;
global resmode;

buffer=zeros(1,2,'uint8');

buffer(1)=uint8(duqxrequests.DUQX_ADC_READ_SINGLE);
buffer(2)=uint8(ch);


fwrite(f,buffer,'uint8');






    if(resmode)

    buffer= fread(f,2,'uint8');


    v_i=bitand(bitor(bitshift(uint16(buffer(2)),8),uint16(buffer(1))),1023);         

    v=((double((v_i)*vref)/1024.0));

    
    else
        
      buffer= fread(f,1,'uint8');
             v_i=bitand(uint16(buffer(1)),255);         

         v=((double((v_i)*vref)/256.0));

    end;


