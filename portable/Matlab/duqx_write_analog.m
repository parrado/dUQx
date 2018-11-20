function duqx_write_analog(v,vref,ch)
%Escribe el voltaje v (nivel de DC de la señal PWM) en el canal ch usando a vref como voltaje de
%referencia.
%
%%Alexander López Parrado(2017)


global duqxrequests;
global f;

if(v>vref) 
    v=vref;
end;

if(v<0) 
    v=0.0;
end;

	

v_i= uint16((1024.0*(v)/vref));


buffer=zeros(1,4,'uint8');

buffer(1)=uint8(duqxrequests.DUQX_DAC_WRITE_SINGLE);
buffer(2)=uint8(bitand(v_i,255));
buffer(3)=uint8(bitand(bitshift(v_i,-8),3));
buffer(4)=uint8(ch);


fwrite(f,buffer,'uint8');
