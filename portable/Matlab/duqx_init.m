function duqx_init(portn)
%Inicia la tarjeta de adquisición de datos.
%
%
%Alexander López Parrado(2017)

global duqxrequests;
global f;
global resmode;

resmode=1;

duqxrequests=[];

duqxrequests.DUQX_ADC_READ_SINGLE=1;
duqxrequests.DUQX_ADC_READ_BUFFER=2;
duqxrequests.DUQX_ADC_PRESCALER_SET=3;
duqxrequests.DUQX_ADC_ENABLED_SET=4;
duqxrequests.DUQX_ADC_RESOLUTION_SET=5;
duqxrequests.DUQX_DAC_WRITE_SINGLE=6;
duqxrequests.DUQX_DIGITAL_CONFIGURE=7;
duqxrequests.DUQX_DIGITAL_WRITE=8;
duqxrequests.DUQX_DIGITAL_READ=9;

if(~isempty(instrfind))
    fclose(instrfind);
end;

if(ispc())
ports={'COM1', 'COM2','COM3','COM4','COM5','COM6','COM7','COM8','COM9',  'COM10', 'COM11', 'COM12','COM13', 'COM14', 'COM15', 'COM16'};
end
if(isunix())
    ports={'/dev/ttyS0','/dev/ttyS1','/dev/ttyS2','/dev/ttyS3','/dev/ttyS4','/dev/ttyS5','/dev/ttyS6','/dev/ttyS7','/dev/ttyS8','/dev/ttyS9','/dev/ttyS10','/dev/ttyS11', '/dev/ttyS12','/dev/ttyS13','/dev/ttyS14','/dev/ttyS15','/dev/ttyUSB0','/dev/ttyUSB1','/dev/ttyUSB2','/dev/ttyUSB3','/dev/ttyUSB4','/dev/ttyUSB5', '/dev/ttyAMA0','/dev/ttyAMA1','/dev/ttyACM0','/dev/ttyACM1', '/dev/rfcomm0','/dev/rfcomm1','/dev/ircomm0','/dev/ircomm1','/dev/cuau0','/dev/cuau1','/dev/cuau2','/dev/cuau3', '/dev/cuaU0','/dev/cuaU1','/dev/cuaU2','/dev/cuaU3'};
end

port=ports{portn+1};

if(isunix)

if unix('ls /dev/ttyS99') == 2
    unix(['sudo ln -s ',port,' /dev/ttyS99']);    
end
port='/dev/ttyS99';
end


f = serial(port,'BaudRate',500000,'DataBits',8);
fopen(f);
pause(3);
