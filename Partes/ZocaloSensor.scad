/*
    Scanner para análisis tridimensional de cobertura vegetal del suelo
    
    Descripción de la pieza: Zócalo para sensor laser VL53L0X. Va atornillado a la base del carro móvil que se desplaza sobre el eje X.
    Cantidad requerida: 1.

    Autor: Matias J. Micheletto    
    Noviembre 2017
    Directores: Rodrigo Santos - Juan Galantini
    Laboratorio de Sistemas Digitales
    Universidad Nacional del Sur - Departamento de Ingeniería Eléctrica y Computadoras    
*/

$fn = 100; // Resolucion del modelo

// Dimensiones exteriores del montaje
W = 24; // Ancho (en direccion X)
L = 45; // Largo (en direccion Y)
H = 7.5;  // Altura (en direccion Z)
e = 1.6;  // Espesor fondo y paredes

// Dimensiones del sensor
Ws = 2.4+0.2; // Ancho (X)
Ls = 4.4+0.2; // Alto (Y)

// Dimensiones y posicion de la plaqueta
Wp = 13.3+0.2; // Ancho (X)
Lp = 10.4+0.2; // Largo (Y)
ep = 1; // Espesor del hundimiento
xp = -2.2;
yp = -2.3;

// Dimensiones y posicion del tornillo de la plaqueta
Rs = 1.3+0.2; // Radio del orificio para el tornillo de la plaqueta
xs = 1.9;
ys = -4.7;

// Tornillos de ajuste
Rp = 1.8; // Radio de los orificios para tornillos
s = 1.5; // Separacion entre el orificio y el borde de la pieza



difference(){
    // Envolvente
    cube([W,L,H], center = true);
    
    // Interior
    translate([0,0,e/2])
        cube([W-2*e,L-4*s-4*Rp,H-e], center = true);
    
    // Agujeros para tornillos
    translate([0,L/2-Rp-s,0])
        cylinder(r = Rp, h = H, center = true);
    translate([0,-L/2+Rp+s,0])
        cylinder(r = Rp, h = H, center = true);
    
    // Orificio para sensor (centrado)
    translate([0,0,-H/2+e/2])
        cube([Ws,Ls,e], center = true);
    
    // Orificio para tornillo de la plaqueta
    translate([xs,ys,-H/2+e/2])
        cylinder(r = Rs, h = e, center = true);
    
    // Hueco para plaqueta    
    translate([xp,yp,-H/2+e-ep/2])
        cube([Wp,Lp,ep], center = true);
        
    // Agujero para sacar los cables
    translate([-W/2+e/2,yp,e/2])
        cube([e,Lp,H-e], center = true);
}