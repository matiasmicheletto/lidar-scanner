/*
    Scanner para análisis tridimensional de cobertura vegetal del suelo
    
    Descripción de la pieza: Soporte para la cadena de desplazamiento horizontal
    Cantidad requerida: 1.

    Autor: Matias J. Micheletto    
    Junio 2018
    Directores: Rodrigo Santos - Juan Galantini
    Laboratorio de Sistemas Digitales
    Universidad Nacional del Sur - Departamento de Ingeniería Eléctrica y Computadoras    
*/

$fn = 100; // Resolucion del modelo

// Dimensiones exteriores del montaje
W = 24; // Ancho (en direccion X)
L = 45; // Largo (en direccion Y)
H = 2; // Alto (en direccion Z)
Hp = 20; // Altura de la pieza vertical
delta = 8.5; // Desplazamiento de la pieza vertical

// Tornillos
Rp = 1.7; // Radio de los orificios para tornillos
s = 1.3; // Separacion entre el orificio y el borde de la pieza

union(){
    difference(){    
        // Base
        translate([0,delta/2,0])
            cube([W,L+delta,H], center = true);

        // Agujeros para tornillos de ajuste al carro
        translate([0,L/2-Rp-s,0])
            cylinder(r = Rp, h = H, center = true);
        translate([0,-L/2+Rp+s,0])
            cylinder(r = Rp, h = H, center = true);
    }

    difference(){
        // Soporte vertical
        translate([0,L/2+H/2+delta,Hp/2-H/2])
            cube([W+6,H,Hp], center = true);

        // Agujeros para tornillos de ajuste a la cadena
        translate([16.5/2,L/2+delta+H/2,Hp/2])
        rotate([90,0,0])
            cylinder(r = Rp, h = H, center = true);
        translate([-16.5/2,L/2+delta+H/2,Hp/2])
        rotate([90,0,0])
            cylinder(r = Rp, h = H, center = true);
    }

    // Soporte
    translate([0,L/2+delta,H/2])
    rotate([0,90,0])
        cylinder(r = H,h = W,center = true);
}