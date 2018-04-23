#include "BYJ48.h"

BYJ48::BYJ48(int In1, int In2, int In3, int In4){
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
    writeStep();
}

void BYJ48::stepForward(BYJ48::STEP_TYPE type){
    // Incrementar step segun tipo de avance
    switch (type) {
      case BYJ48::HS:
        currentStep = currentStep >= 7 ? 0 : currentStep+1;
        break;
      case BYJ48::FSLT:
        currentStep = currentStep >= 11 ? 8 : currentStep+1;
        break;
      case BYJ48::FSHT:
        currentStep = currentStep >= 15 ? 12 : currentStep+1;
        break;
      default:
        break;
    }
    writeStep();
}

void BYJ48::stepBackward(BYJ48::STEP_TYPE type){
    // Decrementar step segun tipo de avance
    switch (type) {
      case BYJ48::HS:
        currentStep = currentStep <= 0 ? 7 : currentStep-1;
        break;
      case BYJ48::FSLT:
        currentStep = currentStep <= 8 ? 11 : currentStep-1;
        break;
      case BYJ48::FSHT:
        currentStep = currentStep <= 12 ? 15 : currentStep-1;
        break;
      default:
        break;
    }
    writeStep();
}

void BYJ48::writeStep(){
    digitalWrite(inputPins[0], sequence[currentStep][0]);
    digitalWrite(inputPins[1], sequence[currentStep][1]);
    digitalWrite(inputPins[2], sequence[currentStep][2]);
    digitalWrite(inputPins[3], sequence[currentStep][3]);
}
