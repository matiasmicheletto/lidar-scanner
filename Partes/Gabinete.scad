
$fn = 100; // Resolucion del modelo

// Dimensiones gabinete
L = 220; // Largo (X)
H = 35; // Alto (Y)
W = 104; // Ancho (Z)
e = 3; // Espesor de paredes

// Dimensiones soportes de ajuste
rs = 2; // Radio de los tornillos
ls = 10; // Largo (X)
hs = 13; // Alto (Y)
ws = e; // Ancho (Z)

module soporte(){ // Soporte para atornillar
    difference(){
        cube([ls,hs,ws], center = true); // Marco

        translate([0,-2,0]) // Orificio
            cylinder(r = rs, h = e, center = true);
    }
}

union(){
    difference(){
        // Gabinete
        cube([L,H,W], center = true);
        
        // Interior
        translate([0,-e,0])
            cube([L-2*e,H-e,W-2*e], center = true);
        
        // Salida de cables
        translate([-40,-H/2+2.5,-W/2+e/2]) // Vcc
            cube([10,5,e], center = true);
        translate([0,-H/2+2.5,-W/2+e/2]) // I2C
            cube([10,5,e], center = true);
        translate([0,-H/2+2.5,20]) // Motores y EndStops
            cube([L,5,10], center = true);
    }

    translate([-55,-hs/2-H/2,W/2-e/2])
        soporte();

    translate([55,-hs/2-H/2,W/2-e/2])
        soporte();

    translate([-55,-hs/2-H/2,-W/2+e/2])
        soporte();

    translate([55,-hs/2-H/2,-W/2+e/2])
        soporte();
}