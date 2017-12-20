// Ejemplo para encender o apagar el pin digital 5 via bluetooth con los comandos 'r' y 's'

void setup() {
  Serial.begin(9600);
  pinMode(5,OUTPUT);
}

void loop() {    
  if (Serial.available()){
    delay(10);
    char c = Serial.read();
    switch(c){
      case 'r': 
        digitalWrite(5,HIGH);
        Serial.println("Encendido");
        break;
      case 's': 
        digitalWrite(5,LOW);
        Serial.println("Apagado");
        break;
      default:
        break;        
    }
  }
}
