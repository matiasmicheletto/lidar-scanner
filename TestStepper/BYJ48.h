#ifndef BYJ48_h
#define BYJ48_h

#include <Arduino.h>

class BYJ48 {
public:
    BYJ48(int In1, int In2, int In3, int In4); // Constructor donde se indican los pines

    void stepCW();                             // Avanzar un step
    void stepCCW();                            // Retroceder un step

private:
    void writeStep();                          // Actualizar salida
    int inputPins[4];                          // Numeros de los pines
    int currentStep;                           // Paso actual
    static const boolean sequences[8][4];      // Secuencias de pasos
};

#endif //BYJ48
