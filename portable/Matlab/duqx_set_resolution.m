function duqx_set_resolution(r)
%Fija el tama�o de bits por muestra capturada en el canal analógico en 8 bits
%(r=0) o 10 bits (r=1).
%
%El tama�o por defecto usado por dUQx es de 10 bits
%
%Alexander L�pez Parrado(2017)



global duqxrequests;
global f;
global resmode;

resmode=r;

r=uint8(r);

buffer=zeros(1,2,'uint8');

buffer(1)=duqxrequests.DUQX_ADC_RESOLUTION_SET;
buffer(2)=bitand(r,255);


fwrite(f,buffer,'uint8');
