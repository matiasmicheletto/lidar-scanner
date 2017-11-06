#ifndef BYJ48_h
#define BYJ48_h

class BYJ48 {
public:
    BYJ48(int In1, int In2, int In3, int In4); // Constructor donde se indican los pines

    enum STEP_TYPE{HS, FSLT, FSHT};           // Tipos de avance

    void stepForward(STEP_TYPE type = HS);    // Avanzar un step
    void stepBackward(STEP_TYPE type = HS);   // Retroceder un step

private:
    void writeStep();

    int inputPins[4];                          // Numeros de los pines
    int currentStep;                           // Paso actual
    static const boolean sequences[16][4];     // Secuencias de pasos
};

// Secuencias
const boolean BYJ48::sequences[16][4] = {
                      // half-step 0-7
                      {false, false, false, true },
                      {false, false, true, true},
                      {false, false, true, false },
                      {false, true, true, false},
                      {false, true, false, false },
                      {true, true, false, false},
                      {true, false, false, false },
                      {true, false, false, true},
                      // full-step low torque 8-11
                      {false, false, false, true },
                      {false, false, true, false },
                      {false, true, false, false },
                      {true, false, false, false },
                      // full-step high torque 12-15
                      {false, false, true, true },
                      {false, true, true, false },
                      {true, true, false, false },
                      {true, false, false, true }
                    };

#endif //BYJ48
