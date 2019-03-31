 /*
    Scanner para análisis tridimensional de cobertura vegetal del suelo
    
    Descripción de la pieza: Tope para sensor de fin de carrera de direccion y
    Cantidad requerida: 1.

    Autor: Matias J. Micheletto    
    Noviembre 2017
    Directores: Rodrigo Santos - Juan Galantini
    Laboratorio de Sistemas Digitales
    Universidad Nacional del Sur - Departamento de Ingeniería Eléctrica y Computadoras    
*/


$fn = 100; // Resolucion del modelo

// Dimensiones exteriores del montaje
W = 25; // Ancho (en direccion X)
L = 10; // Largo (en direccion Y)
H = 10; // Alto (en direccion Z)

r2 = 1.7; // Radio de los orificios para tornillos
d = 17.8; // Separacion entre los orificios

difference(){    
    cube([W,L,H],center = true);
    
    translate([-d/2,0,0])   
        cylinder(r = r2, h = H, center = true);
    translate([d/2,0,0])   
        cylinder(r = r2, h = H, center = true);
}