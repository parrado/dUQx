function vref=duqx_calibrate_analog()
%Retorna en vref el voltaje referencia para los canales an�logos. 
%
%Alexander López Parrado(2017)

duqx_read_analog_single(14,1.0);
vref=1.1/duqx_read_analog_single(14,1.0);
