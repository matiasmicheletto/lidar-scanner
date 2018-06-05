clear; close all; clc

% Cargar datos
filename1 = 'Ensayo_oficina_luz.csv';
data1 = load(filename1);

filename2 = 'Ensayo_oficina_oscuro.csv';
data2 = load(filename2);

% Filtrado
idx = 1;
for ii=1500:2000
    if -data1(ii,3) < -10
        z1(idx) = data1(ii,3);
        idx=idx+1;
    end
end

m1 = mean(z1)
s1 = std(z1)

idx = 1;
for ii=1:500
    if -data2(ii,3) < -10
        z2(idx) = data2(ii,3);
        idx=idx+1;
    end
end

m2 = mean(z2)
s1 = std(z2)

plot(z1,'b.-')
hold on
plot(z2,'r.-')
grid
legend('Con luz','Sin luz');
xlabel('Datos','FontSize', 18);
ylabel('Altura (mm)','FontSize', 18)
set(gca,'FontSize', 18);
