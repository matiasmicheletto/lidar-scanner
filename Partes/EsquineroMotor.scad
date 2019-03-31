/*
    Scanner para análisis tridimensional de cobertura vegetal del suelo
    
    Descripción de la pieza: Soporte para las guias de desplazamiento Y y para los motores paso a paso. Posee espacio para ajustar los interruptores de fin de carrera.
    Cantidad requerida: 2.

    Autor: Matias J. Micheletto    
    Noviembre 2017
    Directores: Rodrigo Santos - Juan Galantini
    Laboratorio de Sistemas Digitales
    Universidad Nacional del Sur - Departamento de Ingeniería Eléctrica y Computadoras    
*/

$fn = 100;  // Resolucion del modelo

// Dimensiones exteriores del montaje
W = 57;     // Ancho (en direccion X)
L = 40;     // Largo (en direccion Y)
d = -19;      // Desplazamiento lateral

Ry = 4.1;   // Radio del orificio para eje y

ls = 4;     // Espesor de pared posterior
ds = 8;     // Ancho hacia los costados
Rs = 2.25;  // Radio de los agujeros para soportes

// Dimensiones del motor
dm = 39-12.25; // Separacion entre el eje Y y el eje del motor
Rm = 14+0.1; // Radio del motor
r2 = 1.7; // Radio de los agujeros para tornillos
d2 = 35; // Separacion entre los tornillos

// Espacio para soporte de cables
wc1 = 3+0.2; // Largo parte ancha
lc1 = 17+0.2; // Ancho parte ancha
xc1 = -dm+sqrt(Rm*Rm-lc1*lc1/4)+wc1/2; // Posicion del centro del cuadrado
wc2 = 5+0.2; // Largo parte angosta
lc2 = 14.5+0.2; // Ancho parte angosta
xc2 = -dm+sqrt(Rm*Rm-lc2*lc2/4)+wc2/2; // Posicion del centro del cuadrado
wc3 = 7; // Largo parte angosta
lc3 = 6; // Ancho parte angosta
xc3 = -dm+sqrt(Rm*Rm-lc3*lc3/4)+wc3/2; // Posicion del centro del cuadrado

// Ajuste del endstop
d3 = 19; // Separacion entre los tornillos

module pieza(H){ // Se pasa como argumento la altura de la pieza para dibujar una de las mitades mas alta
    difference(){ 
        // Envolvente        
        union(){
            translate([d,0,0])
                cube([W,L,H],center = true);
            translate([d,L/2-ls/2+ls,0])
                cube([W+2*ds,ls,H], center = true);
        }
            
        // Espacio para la guia de acero
        translate([0,-L/2+21/2,0])
        rotate([90,0,0])              
            cylinder(r = Ry, h = 21, center = true);                
           
        // Hueco para el motor
        translate([-dm,0,0])
            cylinder(r = Rm, h = H, center = true);
           
        // Orificios para los tornillos del motor        
        translate([-dm-d2/2,0,0])
            cylinder(r = r2, h = H, center = true);    
        translate([-dm+d2/2,0,0])
            cylinder(r = r2, h = H, center = true);
        
        // Espacio para los conectores del motor
        translate([-dm,xc1+dm,0])
            cube([lc1,wc1,H], center = true);
        translate([-dm,xc2+dm,0])
            cube([lc2,wc2,H], center = true);
        translate([-dm,xc3+dm,0])
            cube([lc3,wc3,H], center = true);    
        
        // Agujeros de soporte
        translate([W/2+ds/2+d,L/2+ls/2,-H/4])
        rotate([90,0,0])
            cylinder(r = Rs, h = ls, center = true);
        translate([-W/2-ds/2+d,L/2+ls/2,-H/4])
        rotate([90,0,0])
            cylinder(r = Rs, h = ls, center = true);  
        /*translate([W/2+ds/2+d,L/2+ls/2,H/4])
        rotate([90,0,0])
            cylinder(r = Rs, h = ls, center = true);
        translate([-W/2-ds/2+d,L/2+ls/2,H/4])
        rotate([90,0,0])
            cylinder(r = Rs, h = ls, center = true);*/
        
        
        // Agujeros para tornillos del endstop    
        translate([W/2-r2+d-0.5,-L/2+r2+0.5,0])   
            cylinder(r = r2, h = H, center = true);
        translate([W/2-r2+d-0.5-d3,-L/2+r2+0.5,0])   
            cylinder(r = r2, h = H, center = true);                
        
        // Dibujar solo la mitad de abajo
        translate([d,ls/2,H/4])
            cube([W+2*ds,L+ls,H/2], center = true);              
    }
}

pieza(32);
mirror([0,1,0]) // Reflejar respecto del plano XZ
translate([0,L+2,-6])
    pieza(20); // Uno de los motores debe ir mas alto que el otro
