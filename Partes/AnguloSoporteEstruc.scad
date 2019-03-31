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


// Dimensiones de la base
Wb = 30; // Ancho en direccion X
Lb = 40; // Largo en direccion Y
Hb = 3; // Espesor de la base en direccion Z

// Dimensiones del soporte vertical
Ws = Wb; // Ancho en direccion X
Ls = Hb; // Largo en direccion Y
Hs = 20; // Altura en direccion Z

Rb = 2; // Radio de los orficios
Rr = 2; // Radio del soporte de relleno para el angulo

// Grilla de orificios
Nx = 3; // Cantidad de orificios en direccion X
Ny = 4; // Cantidad de orificios en direccion Y
xMin = -9;
xMax = 9;
yMin = -15;
yMax = 15;


union(){
    difference(){
        // Base
        cube([Wb,Lb,Hb], center = true);
        
        // Grilla de orificios de agarre a la estructura
        for(xPos =[xMin:(xMax-xMin)/(Nx-1):xMax]){
            for(yPos = [yMin:(yMax-yMin)/(Ny-1):yMax]){
                translate([xPos,yPos,0])
                    cylinder(r = Rb, h = Hb, center = true);
            }
        }
    }

    difference(){
        // Soporte vertical
        translate([0,Lb/2+Ls/2,Hs/2-Ls/2])
            cube([Ws,Ls,Hs], center = true);   
        
        // Orificios de soporte 
        translate([Wb/3,Lb/2+Ls/2,Hs/2])
        rotate([90,0,0])
            cylinder(r = Rb, h = Ls, center = true);
        
        translate([-Wb/3,Lb/2+Ls/2,Hs/2])
        rotate([90,0,0])
            cylinder(r = Rb, h = Ls, center = true);
    }
    
    // Soporte de relleno
    translate([0,Lb/2,Hb/2])
    rotate([0,90,0])
        cylinder(r = Rr, h = Wb, center = true);
}