/*
    Scanner para análisis tridimensional de cobertura vegetal del suelo
    
    Descripción de la pieza: Soporte para las guias de desplazamiento Y y para el ajuste de las poleas. Para varillas de 8mm.
    Cantidad requerida: 2.

    Autor: Matias J. Micheletto    
    Noviembre 2017
    Directores: Rodrigo Santos - Juan Galantini
    Laboratorio de Sistemas Digitales
    Universidad Nacional del Sur - Departamento de Ingeniería Eléctrica y Computadoras    
*/

$fn = 100; // Resolucion del modelo

// Dimensiones exteriores del montaje
W = 30; // Ancho (en direccion X)
H = 13; // Alto (en direccion Z)
L = 21; // Largo (en direccion Y)
d = 4; // Desplazamiento del centro de masa

Ry = 4.1; // Radio del orificio para eje y
Rp = 2.25; // Radio orificio para eje de polea

ls = 4; // Espesor de pared posterior
ds = 8; // Ancho hacia los costados
Rs = 2.25; // Radio de los agujeros para soportes

module pieza(){ // Pieza central
    difference(){ 
        // Envolvente        
        translate([-d,0,0])
            union(){
                cube([W,L,H],center = true);
                translate([0,L/2-ls/2,0])
                    cube([W+2*ds,ls,H], center = true);
            }
       
        // Espacio para eje de acero de 8mm
        rotate([90,0,0])     
        translate([-10,0,0])
            cylinder(r = Ry, h = L, center = true);
        
        // Eje de la polea    
        translate([Rp,0,0])
            cylinder(r = Rp, h = H, center = true);          
        
            
        // Agujeros de soporte
        translate([W/2+ds/2-d,L/2-ls/2,-H/4])
        rotate([90,0,0])
            cylinder(r = Rs, h = ls, center = true);
        translate([-W/2-ds/2-d,L/2-ls/2,-H/4])
        rotate([90,0,0])
            cylinder(r = Rs, h = ls, center = true);        
            
        // Dibujar solo la mitad de abajo
        translate([-d,0,H/4])
            cube([W+2*ds,L,H/2], center = true);
    }
}


pieza();
mirror([0,1,0]) // Reflejar respecto del plano XZ
translate([0,L+2,0])
    pieza();