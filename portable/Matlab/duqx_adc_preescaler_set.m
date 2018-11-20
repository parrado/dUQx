function duqx_adc_preescaler_set(ps)
%Fija a ps el preescalador del reloj del ADC
%
%ps == 0, reloj del ADC es 16e6/2
%ps == 1, reloj del ADC es 16e6/2
%ps == 2, reloj del ADC es 16e6/4
%ps == 3, reloj del ADC es 16e6/8
%ps == 4, reloj del ADC es 16e6/16
%ps == 5, reloj del ADC es 16e6/32
%ps == 5, reloj del ADC es 16e6/64
%ps == 7, reloj del ADC es 16e6/128
%
%por defecto el preescalador del ADC es de 128
%
%Alexander L�pez Parrado (2017)


global duqxrequests;
global f;




ps=uint8(ps);

buffer=zeros(1,2,'uint8');

buffer(1)=duqxrequests.DUQX_ADC_PRESCALER_SET;
buffer(2)=bitand(ps,255);


fwrite(f,buffer,'uint8');
