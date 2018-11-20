%Prueba de dUQx sobre MATLAB
%
%Alexander López Parrado (2017)

clc;
clear all;
close all;

%Inicia dUQx


%Si est� conectada inicia la captura
duqx_init(24);
duqx_set_resolution(1);

duqx_configure_digital(0)
duqx_write_digital(713)
duqx_read_digital()




    
ch=5;
    
disp('dUQx est� conectada');
disp('Iniciando test');


vref=duqx_calibrate_analog();

%Vector donde se almacenar�n las muestras
samples=[];

%Cantidad de muestras
Ns=1000;

%Env�a un escal�n....
duqx_write_analog(1.0,vref,0);

% %...e inicia el muestreo
% while(Ns)
%     %Se almacena la lectura an�loga
%     samples=[samples; duqx_read_analog_single(ch,vref)];    
%     %Se pausa durante 10ms
%     pause(10e-3);
%     Ns=Ns-1;
% end;

%Grafica las muestras capturadas
plot(samples);



%Se hace una prueba de tiempo de muestreo a dUQx.
Nit=1000;

tic;
for(k=1:Nit)
duqx_read_analog_single(ch,vref);
%duqx_write_analog(1.0,vref);
end;
t_elapsed=toc();

s=sprintf('Puedo muestrear en promedio a una tasa de: %f kHz.',((Nit)/t_elapsed)/1000);
disp(s);


duqx_end();