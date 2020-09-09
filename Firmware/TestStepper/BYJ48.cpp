#include "BYJ48.h"

// Secuencias de steps
const boolean BYJ48::sequences[8][4] = { 
                      {false, false, false, true },
                      {false, false, true, true},
                      {false, false, true, false },
                      {false, true, true, false},
                      {false, true, false, false },
                      {true, true, false, false},
                      {true, false, false, false },
                      {true, false, false, true}
                   };


BYJ48::BYJ48(int In1, int In2, int In3, int In4, boolean mode){
    // Numero de pines de salida
    inputPins[0] = In1;
    inputPins[1] = In2;
    inputPins[2] = In3;
    inputPins[3] = In4;

    // Modo de pines de salida
    pinMode(inputPins[0], OUTPUT);
    pinMode(inputPins[1], OUTPUT);
    pinMode(inputPins[2], OUTPUT);
    pinMode(inputPins[3], OUTPUT);

    // Disponer salida en step inicial (HS)
    currentStep = 0;
    
    // Configurar modo inicial
    halfSteppingMode = mode;
    
    // Actualizar salidas
    writeStep();
}


void BYJ48::writeStep(){ // Actualiza salida
    digitalWrite(inputPins[0], sequences[currentStep][0]);
    digitalWrite(inputPins[1], sequences[currentStep][1]);
    digitalWrite(inputPins[2], sequences[currentStep][2]);
    digitalWrite(inputPins[3], sequences[currentStep][3]);
}

void BYJ48::setMode(boolean mode){ // Alternar modo half o full
    halfSteppingMode = mode;
    currentStep = 0;
    writeStep();
}

void BYJ48::stepCW(){ // Incrementar paso
    currentStep = currentStep >= (halfSteppingMode ? 7:6) ? 0 : currentStep+(halfSteppingMode ? 1:2);
    writeStep();
}

void BYJ48::stepCCW(){ // Decrementar paso
    currentStep = currentStep <= 0 ? (halfSteppingMode ? 7:6) : currentStep-(halfSteppingMode ? 1:2);
    writeStep();
}



