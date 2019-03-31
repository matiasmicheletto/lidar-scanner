/*
    Scanner para análisis tridimensional de cobertura vegetal del suelo
    
    Descripción de la pieza: Carro de desplazamiento en direccion X. Contiene espacio para rodamientos lineales para varilla de 8mm (tiene 15 mm de diametro por 24 de largo). Debajo de esta pieza se atornilla el zocalo para el sensor láser.
    Cantidad requerida: 2.

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
H = 20; // Alto (en direccion Z)

// Varillas
R = 7.5+0.1; // Radio para rodamiento
d = 9; // Distancia de la varilla al centro (separacion intervarillas/2)

// Tornillos
Rp = 1.5; // Radio de los orificios para tornillos
s = 1.5; // Separacion entre el orificio y el borde de la pieza

difference(){
    // Envolvente de la pieza
    cube([W,L,H], center = true);
    
    // Espacio para las varillas con rodamiento
    rotate([0,90,0])
    translate([0,d,0])
        cylinder(r = R, h = W+1, center = true);
    rotate([0,90,0])
    translate([0,-d,0])
        cylinder(r = R, h = W+1, center = true);
    
    // Agujeros para tornillos
    translate([0,L/2-Rp-s,0])
        cylinder(r = Rp, h = H, center = true);
    translate([0,-L/2+Rp+s,0])
        cylinder(r = Rp, h = H, center = true);
    
    // Dibujar solo la mitad
    translate([0,0,H/4])
        cube([W,L,H/2],center=true);
}
