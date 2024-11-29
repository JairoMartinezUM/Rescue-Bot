#include <ServoTimer2.h>

// Pin Definitions
#define PIN_TRIGGER_ULTRASONICO  8
#define PIN_ECHO_ULTRASONICO     7
#define PIN_MOTOR_A_1            9
#define PIN_MOTOR_A_2            6
#define PIN_MOTOR_B_1            5
#define PIN_MOTOR_B_2            10
#define PIN_BUZZER               12
#define PIN_SERVO_ULTRASONICO    3
#define PIN_SERVO_2              2

// IR Sensor Pins
const int IR_PINS[] = {A0, A1, A2, A3};

// Servo and Color Sensor Setup
ServoTimer2 servoUltrasonico;

// Función de medición de distancia
long medirDistanciaUltrasonico() {
  digitalWrite(PIN_TRIGGER_ULTRASONICO, LOW);
  delayMicroseconds(2);
  digitalWrite(PIN_TRIGGER_ULTRASONICO, HIGH);
  delayMicroseconds(10);
  digitalWrite(PIN_TRIGGER_ULTRASONICO, LOW);
  
  return pulseIn(PIN_ECHO_ULTRASONICO, HIGH) * 0.034 / 2;
}

// Función para detectar pared
bool detectarPared() {
  return medirDistanciaUltrasonico() < 10;
}

// Funciones de movimiento
void avanzar() {
  analogWrite(PIN_MOTOR_A_1, 0);
  analogWrite(PIN_MOTOR_A_2, 150);
  analogWrite(PIN_MOTOR_B_1, 150);
  analogWrite(PIN_MOTOR_B_2, 0);
}

void detener() {
  analogWrite(PIN_MOTOR_A_1, 0);
  analogWrite(PIN_MOTOR_A_2, 0);
  analogWrite(PIN_MOTOR_B_1, 0);
  analogWrite(PIN_MOTOR_B_2, 0);
}

void girarDerecha() {
  analogWrite(PIN_MOTOR_A_1, 0);
  analogWrite(PIN_MOTOR_A_2, 180);
  analogWrite(PIN_MOTOR_B_1, 0);
  analogWrite(PIN_MOTOR_B_2, 180);
}

void girarIzquierda() {
  analogWrite(PIN_MOTOR_A_1, 220);
  analogWrite(PIN_MOTOR_A_2, 0);
  analogWrite(PIN_MOTOR_B_1, 220);
  analogWrite(PIN_MOTOR_B_2, 0);
}

void retroceder() {
  analogWrite(PIN_MOTOR_A_1, 150);
  analogWrite(PIN_MOTOR_A_2, 0);
  analogWrite(PIN_MOTOR_B_1, 0);
  analogWrite(PIN_MOTOR_B_2, 150);
}

void giroMediaVuelta() {
  analogWrite(PIN_MOTOR_A_1, 180);
  analogWrite(PIN_MOTOR_A_2, 0);
  analogWrite(PIN_MOTOR_B_1, 0);
  analogWrite(PIN_MOTOR_B_2, 180);
  delay(600);
  detener();
}

// Escanear direcciones
void escanearDirecciones(bool &paredIzquierda, bool &paredDerecha, bool &paredFrente) {
  servoUltrasonico.write(750);   // 0 grados (izquierda)
  delay(500);
  paredIzquierda = detectarPared();
  
  servoUltrasonico.write(2250);  // 180 grados (derecha)
  delay(500);
  paredDerecha = detectarPared();
  
  servoUltrasonico.write(1500);  // 90 grados (frente)
  delay(500);
  paredFrente = detectarPared();
}

// Tomar decisión de movimiento
void tomarDecisionMovimiento(bool paredIzquierda, bool paredDerecha, bool paredFrente) {
  if (!paredFrente) {
    avanzar();
    delay(1000);
  } else if (!paredDerecha) {
    girarDerecha();
    delay(1000);
  } else if (!paredIzquierda) {
    girarIzquierda();
    delay(1000);
  } else {
    giroMediaVuelta();
  }
  
  servoUltrasonico.write(1500);  // Reiniciar a frente
}

void setup() {
  Serial.begin(9600);
  
  // Configuración de pines
  pinMode(PIN_MOTOR_A_1, OUTPUT);
  pinMode(PIN_MOTOR_A_2, OUTPUT);
  pinMode(PIN_MOTOR_B_1, OUTPUT);
  pinMode(PIN_MOTOR_B_2, OUTPUT);
  pinMode(PIN_BUZZER, OUTPUT);
  
  for (int i = 0; i < 4; i++) {
    pinMode(IR_PINS[i], INPUT);
  }
  
  pinMode(PIN_TRIGGER_ULTRASONICO, OUTPUT);
  pinMode(PIN_ECHO_ULTRASONICO, INPUT);
  
  // Configuración de servos
  servoUltrasonico.attach(PIN_SERVO_ULTRASONICO);
  
  // Esperar 2 segundos
  delay(2000);
  
  // Inicializar servos
  servoUltrasonico.write(1500);  // 90 grados
}

unsigned long ultimaComprobacion = 0;

void loop() {
  // Avanzar constantemente
  avanzar();
  
  // Verificar cruz blanca
  if (digitalRead(IR_PINS[0]) == 1 && 
      digitalRead(IR_PINS[1]) == 1 && 
      digitalRead(IR_PINS[2]) == 1 && 
      digitalRead(IR_PINS[3]) == 1) {
    
    detener();
    delay(1500);
    
    // Escanear direcciones
    bool paredIzquierda, paredDerecha, paredFrente;
    escanearDirecciones(paredIzquierda, paredDerecha, paredFrente);
    
    // Tomar decisión de movimiento
    tomarDecisionMovimiento(paredIzquierda, paredDerecha, paredFrente);
  }
  
  // Comprobar pared cada 2 segundos
  unsigned long tiempoActual = millis();
  if (tiempoActual - ultimaComprobacion >= 2000) {
    if (detectarPared()) {
      detener();
      
      // Escanear direcciones
      bool paredIzquierda, paredDerecha, paredFrente;
      escanearDirecciones(paredIzquierda, paredDerecha, paredFrente);
      
      // Tomar decisión de movimiento
      tomarDecisionMovimiento(paredIzquierda, paredDerecha, paredFrente);
    }
    
    // Actualizar la última comprobación
    ultimaComprobacion = tiempoActual;
  }
}