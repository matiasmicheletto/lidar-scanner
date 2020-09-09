 /*
    Scanner para análisis tridimensional de cobertura vegetal del suelo
    
    Descripción de la pieza: Montaje del carro de desplazamiento X que desliza sobre las guias de desplazamiento Y. 
    Cantidad requerida: 4.

    Autor: Matias J. Micheletto    
    Noviembre 2017
    Directores: Rodrigo Santos - Juan Galantini
    Laboratorio de Sistemas Digitales
    Universidad Nacional del Sur - Departamento de Ingeniería Eléctrica y Computadoras    
*/
 
 
$fn = 100; // Resolucion del modelo

// Dimensiones exteriores del montaje
W = 40; // Ancho (en direccion X)
L = 45; // Largo (en direccion Y)
H = 20; // Alto (en direccion Z)

// Eje de direccion y
Ry = 7.6; // Radio del orificio para rodamientos
Ry2 = 4.5; // Radio del orificio para el eje

// Ejes de direccion x
Rx = 4.1; // Radio de los orificios para varillas de desplazamiento en direccion X
d = 9; // Distancia la varilla al centro (separacion intervarillas/2)

Rp = 2.25; // Radio orificio para eje de polea

// Ejuste del endstop
r2 = 1.7; // Radio del orificio para tornillo del endstop
d2 = 19; // Separacion entre los tornillos del endstop

module pieza(){
    difference(){
        // Envolvente
        cube([W,L,H], center = true);

        // Espacio para rodamiento lineal
        rotate([90,0,0])
        translate([-W/4,0,0])
            cylinder(r = Ry, h = 25, center = true);
        
        // Espacio para el eje
        rotate([90,0,0])
        translate([-W/4,0,0])
            cylinder(r = Ry2, h = L+1, center = true);

        // Varillas de desplazamiento x
        rotate([0,90,0])
        translate([0,d,W/2])
            cylinder(r = Rx, h = W, center = true);
        rotate([0,90,0])
        translate([0,-d,W/2])
            cylinder(r = Rx, h = W, center = true);

        // Eje de la polea
        translate([Rp,0,0])
            cylinder(r = Rp, h = H, center = true);

        // Agujeros para tornillos del endstop    
        /*translate([W/2-r2-0.5,0,0])   
            cylinder(r = r2, h = H, center = true);
        translate([W/2-r2-0.5,d2,0])   
            cylinder(r = r2, h = H, center = true);
        // El que sigue no es necesario pero agrego por simetria
        translate([W/2-r2-0.5,-d2,0])   
            cylinder(r = r2, h = H, center = true);*/

        // Agujeros para tope del endstop en direccion y
        translate([0,-d2,0])   
            cylinder(r = r2, h = H, center = true);
        translate([-W/2+r2+0.5,-d2,0])   
            cylinder(r = r2, h = H, center = true);
        /*translate([0,d2,0])   
            cylinder(r = r2, h = H, center = true);
        translate([-W/2+r2+0.5,d2,0])   
            cylinder(r = r2, h = H, center = true);*/

        // Dibujar solo la mitad
        translate([0,0,H/4])
            cube([W,L,H/2],center=true);
    }
}

pieza();
mirror([0,1,0]) // Reflejar respecto del plano XZ
translate([0,L+2,0])
    pieza(); 

