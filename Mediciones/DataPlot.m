clear; close all; clc

% Cargar datos
filename = 'Registro_Ensayo_1.csv';
data = load(filename);

x = data(:,2)'/15000*500;
y = data(:,1)'/15000*500;

% Filtrado
idx = 1;
for ii=1:size(data,1)
    if -data(ii,3) > -1000
        z(ii) = 700-data(ii,3);
        idx=idx+1;
    else
        z(ii) = 0;
    end
end

% Calcular volumen ???
[tri_idx, v] = convhull(x,y,z);

disp(filename);
disp(['Datos descartados: ',num2str(100 - idx/size(data,1)*100),'%']);
disp(['Volumen: ',num2str(v/1000),' cm^3']);
disp(['Promedio: ',num2str(mean(z)),' mm']);
disp(['Dispersión: ',num2str(std(z)),' mm^2']);


tri = delaunay(x,y);

%figure;
%trisurf(tri, x, y, z);
%axis vis3d

figure;
trisurf(tri, x, y, z);
light('Position',[-50 -15 29]);
lighting gouraud
shading interp
colorbar EastOutside
xlabel('Dirección horizontal')
ylabel('Dirección vertical')
zlabel('Altura (mm)')


%figure;
%plot3(x,y,z,'.');
%grid;