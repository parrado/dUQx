%Prueba de dUQx sobre MATLAB (captura en modo bloque)
%
%Alexander López Parrado (2017)

clc;
clear all;
close all;



duqx_init(24);
duqx_set_resolution(1);
Nf=3;
Ns=1000;

ch=14;

disp('dUQx está conectada');

vref=duqx_calibrate_analog();






samples=[];


i=Nf;
tic;
while(i)
    
    %Vector donde se almacenarán las muestras
    
    block=duqx_read_analog_buffer(ch,vref,Ns);
    
    samples=[samples block];
    i=i-1;
    
    
end

t_elapsed=toc();

s=sprintf('Puedo muestrear en promedio a una tasa de: %f kHz.',((numel(samples))/t_elapsed)/1000);
disp(s);


plot(samples);

duqx_end();