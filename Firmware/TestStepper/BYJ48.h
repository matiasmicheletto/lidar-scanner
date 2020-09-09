#ifndef BYJ48_h
#define BYJ48_h

#include <Arduino.h>

class BYJ48 {
public:
    BYJ48(int In1, int In2, int In3, int In4, boolean mode); // Constructor donde se indican los pines y modo

    void stepCW();                             // Avanzar un step
    void stepCCW();                            // Retroceder un step
    
    void setMode(boolean mode);                // Alternar modo full o half stepping

private:
    int inputPins[4];                          // Numeros de los pines
    int currentStep;                           // Paso actual
    static const boolean sequences[8][4];      // Secuencias de pasos
    boolean halfSteppingMode;                  // Modo halfstepping o fullstepping

    void writeStep();                          // Actualizar salida
};

#endif //BYJ48
