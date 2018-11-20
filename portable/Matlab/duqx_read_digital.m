function pin=duqx_read_digital()
%Lee el puerto digital y retorna su estado en pin
%
%Alexander López Parrado(2017)

global duqxrequests;
global f;


fwrite(f,uint8(duqxrequests.DUQX_DIGITAL_READ),'uint8');




buffer= fread(f,2,'uint8');


pin=bitand(bitor(bitshift(uint16(buffer(2)),8),uint16(buffer(1))),1023);



