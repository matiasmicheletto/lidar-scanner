#ifndef BYJ48_h
#define BYJ48_h

#include <Arduino.h>

class BYJ48 {
public:
    BYJ48(int In1, int In2, int In3, int In4); // Constructor donde se indican los pines

    void enable();                             // Energizar (frenar)
    void disable();                            // Desenergizar
    void stepCW();                             // Avanzar un step
    void stepCCW();                            // Retroceder un step

private:
    void writeStep();                          // Actualizar salida
    int outputPins[4];                         // Numeros de los pines
    int currentStep;                           // Paso actual
    static const boolean sequences[8][4];      // Secuencias de pasos
};

// Secuencias
const boolean BYJ48::sequences[8][4] = {
                      // half-step 0-7
                      {false, false, false, true },
                      {false, false, true, true},
                      {false, false, true, false },
                      {false, true, true, false},
                      {false, true, false, false },
                      {true, true, false, false},
                      {true, false, false, false },
                      {true, false, false, true}
                    };

BYJ48::BYJ48(int In1, int In2, int In3, int In4){
    // Numero de pines de salida
    outputPins[0] = In1;
    outputPins[1] = In2;
    outputPins[2] = In3;
    outputPins[3] = In4;

    // Modo de pines de salida
    pinMode(outputPins[0], OUTPUT);
    pinMode(outputPins[1], OUTPUT);
    pinMode(outputPins[2], OUTPUT);
    pinMode(outputPins[3], OUTPUT);

    // Disponer salida en step inicial (HS)
    currentStep = 0;
    writeStep();
}

void BYJ48::enable(){
    // Alimentar bobinas para que quede frenado
    writeStep();  
}

void BYJ48::disable(){
    // Desalimentar todas para que quede liberado
    digitalWrite(outputPins[0], LOW);
    digitalWrite(outputPins[1], LOW);
    digitalWrite(outputPins[2], LOW);
    digitalWrite(outputPins[3], LOW);
}

void BYJ48::stepCW(){
    // Incrementar paso
    currentStep = currentStep >= 7 ? 0 : currentStep+1;
    writeStep();
}

void BYJ48::stepCCW(){
    // Decrementar paso
    currentStep = currentStep <= 0 ? 7 : currentStep-1;
    writeStep();
}

void BYJ48::writeStep(){
    digitalWrite(outputPins[0], sequences[currentStep][0]);
    digitalWrite(outputPins[1], sequences[currentStep][1]);
    digitalWrite(outputPins[2], sequences[currentStep][2]);
    digitalWrite(outputPins[3], sequences[currentStep][3]);
}


#endif //BYJ48
