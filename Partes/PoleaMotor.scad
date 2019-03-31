/*
    Scanner para análisis tridimensional de cobertura vegetal del suelo
    
    Descripción de la pieza: Polea de 40mm de diametro para motor paso a paso BYj48.
    Cantidad requerida: 2.

    Autor: Matias J. Micheletto    
    Noviembre 2017
    Directores: Rodrigo Santos - Juan Galantini
    Laboratorio de Sistemas Digitales
    Universidad Nacional del Sur - Departamento de Ingeniería Eléctrica y Computadoras    
*/

$fn = 100;      // Resolucion del modelo

H = 6;        // Altura
Rp = 2;       // Radio de la pista (menor que H/2)
R = 22+Rp;    // Radio exterior (original 19.5+Rp)

Ra = 2.50;      // Radio eje
Rb = 1.55;      // Achatamiento

// Orificios
Nh = 6;         // Cantidad
Rh = 5.6;         // Radio (original 5)
Dh = R/2+1;     // Separacion desde el centro

// Ahuecamiento
Di = 2;         // Borde interior
De = 4;         // Borde exterior
e = 2;          // Profundidad

    
    

difference(){
    cylinder(r = R, h = H); // Envolvente

    intersection(){
        cylinder(r = Ra, h = H); // Eje
        translate([0,0,H/2])
            cube([Rb*2,Ra*2,H], center = true);
    }

    // Pista para la cuerda convexa
    rotate_extrude(convexity = 10)
    translate([R, H/2, 0])
        circle(r = Rp);  
    
    // Pista para cuerda recta
    //translate([0,0,(H-Rp*2)/2])
    //difference(){
    //    cylinder(r = R, h = Rp*2);
    //    cylinder(r = R-Rp, h = Rp*2);
    //}
    

    // Orificios
    for(angle = [0 : 360/Nh : 360])
        translate([Dh*sin(angle),Dh*cos(angle),0])
            cylinder(r = Rh, h = H); 

    // Ahuecamiento superior
    translate([0,0,H-e])
        difference(){
            cylinder(r = R-De, h = e);
            cylinder(r = Ra+Di, h = e);
        }
}
