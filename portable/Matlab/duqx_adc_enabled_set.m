function duqx_adc_enabled_set(e)
%Habilita (e=1) o inhabilita (e=0)  el ADC
%
%%Alexander López Parrado (2017)

global duqxrequests;
global f;




e=uint8(e);

buffer=zeros(1,2,'uint8');

buffer(1)=duqxrequests.DUQX_ADC_ENABLED_SET;
buffer(2)=bitand(e,255);


fwrite(f,buffer,'uint8');
