clear; close all; clc 

% Cargar datos
data = load('RegScanner.csv');
y = data(:,1); x = data(:,2); z = -data(:,3);

tri = delaunay(x,y);
[r,c] = size(tri);


figure;
trisurf(tri, x, y, z);
%axis vis3d

figure;
trisurf(tri, x, y, z);
%light('Position',[-50 -15 29]);
%lighting phong
shading interp
colorbar EastOutside

figure;
plot3(x,y,z,'.');
grid;
