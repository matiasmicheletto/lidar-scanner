clear all; close all; clc;

portname = '/dev/ttyUSB0'; % Verificar!

delete(instrfind({'Port'},{portname}));
port = serial(portname);
port.BaudRate = 9600;
warning('off','MATLAB:serial:fscanf:unsuccessfulRead');
fopen(port); 
fprintf(port,'a'); % Iniciar

pause(1);

n = 1; 
while 1 
   data(n,:) = str2num(fscanf(port))';
   n = n+1;
end

scatter3(data(:,1),data(:,2),data(:,3),'filled');
fprintf(port,'b'); % Detener (para cancelar)
fclose(port); delete(port); clear port;