#include <Adafruit_TCS34725.h>
#include <ServoTimer2.h>

#define TRIGGER_PIN  8
#define ECHO_PIN 7

Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_1X);

#define MOTOR_A_1 6
#define MOTOR_A_2 9
#define MOTOR_B_1 10
#define MOTOR_B_2 5

#define IR_SENSOR_1 A0
#define IR_SENSOR_2 A1
#define IR_SENSOR_3 A2
#define IR_SENSOR_4 A3
const int umbralIR = 500;

ServoTimer2 servoMotorU;

long distanciaIzq = 0, distanciaCen = 0, distanciaDer = 0;

void setup() {
  Serial.begin(9600);
  pinMode(IR_SENSOR_1, INPUT);
  pinMode(IR_SENSOR_2, INPUT);
  pinMode(IR_SENSOR_3, INPUT);
  pinMode(IR_SENSOR_4, INPUT);

  pinMode(MOTOR_A_1, OUTPUT);
  pinMode(MOTOR_A_2, OUTPUT);
  pinMode(MOTOR_B_1, OUTPUT);
  pinMode(MOTOR_B_2, OUTPUT);

  tcs.begin();

  pinMode(TRIGGER_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  servoMotorU.attach(3);
  servoMotorU.write(1500);
}

void leerIR(){
  int valoresAnalogicos[4];
  int valoresDigitales[4];

  for(int i=0; i<4; i++){
    valoresAnalogicos[i] = analogRead(sensores[i]);
    valoresDigitales[i] = (valoresAnalogicos[i] > umbral)? 0:1;
  }
}

long leerDistancia(){
  digitalWrite(TRIGGER_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIGGER_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGGER_PIN, LOW);

  long duracion = pulseIn(ECHO_PIN, HIGH);
  
  long distancia = duracion * 0.0344 / 2;
  return distancia;
}

bool detectarBlanco() {
  int irValores[4] = {
    analogRead(IR_SENSOR_1),
    analogRead(IR_SENSOR_2),
    analogRead(IR_SENSOR_3),
    analogRead(IR_SENSOR_4)
  };

  for (int i = 0; i < 4; i++) {
    if (irValores[i] > umbralIR) {
      return true;
    }
  }
  return false;
}

void escanearParedes(){
  int angulos[] = {750, 1500, 2550}; //0° 90° 180°
  int umbralDistancia = 8;

  servoMotorU.write(angulos[1]);
  delay(500);
  distanciaCen = leerDistancia();

  servoMotorU.write(angulos[2]);
  delay(500);
  distanciaDer = leerDistancia();

  servoMotorU.write(angulos[0]);
  delay(500);
  distanciaIzq = leerDistancia();

  if (distanciaCen >= umbralDistancia) {
    avanzar();
  } else if (distanciaDer >= umbralDistancia) {
    derecha();
  } else if (distanciaIzq >= umbralDistancia) {
    izquierda();
  } else {
    detener();
  }
  servoMotorU.write(1500);
}

void avanzar(){
  analogWrite(6,120);
  analogWrite(9,0);
  analogWrite(10,0);
  analogWrite(5,140);
}

void detener(){
  analogWrite(6,0);
  analogWrite(9,0);
  analogWrite(10,0);
  analogWrite(5,0);
}

void derecha(){
  analogWrite(6,0);
  analogWrite(9,120);
  analogWrite(10,0);
  analogWrite(5,120); 
}

void izquierda(){
  analogWrite(6,120);
  analogWrite(9,0);
  analogWrite(10,120);
  analogWrite(5,0);
}

void loop() {
  if (detectarBlanco()) {
    detener();
    escanearParedes();
  } else {
    avanzar();
    delay(2000);
  }
  delay(200);
}