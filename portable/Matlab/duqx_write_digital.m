function duqx_write_digital(port)
%Escribe el valor de port en el puerto digital
%
%Alexander López Parrado(2017)


global duqxrequests;
global f;

port=uint16(port);

buffer=zeros(1,3,'uint8');

buffer(1)=duqxrequests.DUQX_DIGITAL_WRITE;
buffer(2)=bitand(port,255);
buffer(3)=bitand(bitshift(port,-8),3);

fwrite(f,buffer,'uint8');
