function duqx_configure_digital(ddr)
%Configura el puerto digital usando como registro de direcci�n de datos a
%ddr. Un bit en 1 en ddr configura como salida el pin respectivo, de lo
%contrario lo configura como entrada.
%
%Alexander López Parrado(2017)

global duqxrequests;
global f;

ddr=uint16(ddr);

buffer=zeros(1,3,'uint8');

buffer(1)=duqxrequests.DUQX_DIGITAL_CONFIGURE;
buffer(2)=bitand(ddr,255);
buffer(3)=bitand(bitshift(ddr,-8),3);

fwrite(f,buffer,'uint8');
