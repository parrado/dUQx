function v=duqx_read_analog_buffer(ch,vref,n)
%Intenta leer hasta n muestras del canal analógico ch usando vref como
%voltaje de referencia.
%
%n debe ser menor o igual a 250 para el modo de 8 bits por muestra.
%n debe ser menor o igual a 125 para el modo de 10 bits por muestra.
%
%Las muestras capturadas son retornadas en el vector v.
%
%%Alexander L�pez Parrado(2017)

global duqxrequests;
global f;
global resmode;

buffer=zeros(1,4,'uint8');

 if(resmode)
     if(n>(f.InputBufferSize/2))
         n=f.InputBufferSize/2;
     end;
 else
     if(n>f.InputBufferSize)
         n=f.InputBufferSize;
     end;
 
 end;


buffer(1)=uint8(duqxrequests.DUQX_ADC_READ_BUFFER);
buffer(2)=uint8(ch);
buffer(3)=uint8(bitand(n,255));
buffer(4)=uint8(bitand(bitshift(n,-8),255));


fwrite(f,buffer,'uint8');


if(resmode)
    
    
    
    buffer= fread(f,2*n,'uint8');
    
    
    
    v=zeros(1,numel(buffer)/2);
    
    for i=0:((numel(buffer)/2)-1)
        v_i=bitand(bitor(bitshift(uint16(buffer(2*i+2)),8),uint16(buffer(2*i+1))),1023);
        v(i+1)=((double((v_i)*vref)/1024.0));
    end
    
    
    
    
else
        
    buffer= fread(f,n,'uint8');
    
    
    
    v_i=bitand(uint16(buffer),255);
    v=((double((v_i)*vref)/256.0))';
    
    
    
    
end

end










