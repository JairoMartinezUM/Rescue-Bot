#include "Adafruit_TCS34725.h"
#include <ServoTimer2.h>;
#define TRIGGER_PIN 8
#define ECHO_PIN 7

Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_1X);
ServoTimer2 servoMotorU;
ServoTimer2 servoMotorO;

// Definición del mapa y la pila
const int filas = 21;
const int columnas = 11;
char mapa[filas][columnas];
const int MAX_SIZE = 100;
int stackX[MAX_SIZE];
int stackY[MAX_SIZE];
int top = -1;

// Variables de posición inicial
int posX = 7;
int posY = 21;

// Funciones de la pila
bool isEmpty() { return top == -1; }
bool isFull() { return top >= MAX_SIZE - 1; }
bool push(int x, int y) {
    if (isFull()) return false;
    stackX[++top] = x;
    stackY[top] = y;
    return true;
}
bool pop() {
    if (isEmpty()) return false;
    top--;
    return true;
}
int peekX() { return isEmpty() ? -1 : stackX[top]; }
int peekY() { return isEmpty() ? -1 : stackY[top]; }

// Funciones auxiliares
void imprimirMapa() {
    Serial.println("Mapa actualizado:");
    for (int i = 0; i < filas; i++) {
        for (int j = 0; j < columnas; j++) {
            Serial.print(mapa[i][j]);
        }
        Serial.println();
    }
}

long distanciaCalculada() {
    digitalWrite(TRIGGER_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIGGER_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIGGER_PIN, LOW);

    long duracion = pulseIn(ECHO_PIN, HIGH);
    return (duracion * 0.034) / 2;
}

String detectarColor() {
    uint16_t r, g, b, c;
    tcs.getRawData(&r, &g, &b, &c);
    if (c > 500) {
        if (g > r && g > b) return "verde";
        else if (r > g && r > b) return "rojo";
    }
    return "ninguno";
}

void setup() {
    Serial.begin(9600);
    pinMode(TRIGGER_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);

    // Inicialización de los servos
    servoMotorU.attach(6);
    servoMotorO.attach(7);
    servoMotorU.write(90);
    servoMotorO.write(0);

    // Inicialización del mapa
    for (int i = 0; i < filas; i++) {
        for (int j = 0; j < columnas; j++) {
            mapa[i][j] = '?';
        }
    }
    mapa[posY][posX] = '0';
    push(posX, posY);

    if (!tcs.begin()) {
        Serial.println("Error: Sensor de color no encontrado");
        while (1);
    }
}

void loop() {
    String colorDetectado = detectarColor();
    if (colorDetectado == "verde") {
        mapa[posY][posX] = '0';
        push(posX, posY);
    } else if (colorDetectado == "rojo") {
        mapa[posY][posX] = 'M';

  avanzar();
    delay(2500);

    detener();
    servoMotorO.write(90);
    Serial.println("pinza cerrada");

    }else{
      Serial.println("Color no valido");
      
      mvuelta();
     delay(2500);
     detener();
     servoMotorU.write(180);
     Serial.println(" Girando haciendo la Media vuelta");
    }
     


    long distancia = distanciaCalculada();
    if (distancia < 10) {
      Serial.println("Pared detectada");
        if (posX >= 0 && posX < columnas && posY >= 0 && posY < filas) {
            mapa[posY][posX] = '|';
        }
    } else {
      if (posX >=0 && posX < columnas && posY >= 0 && posY < filas) {
        mapa[posY][posX]= ' ';
      }
    }
    
    imprimirMapa();
    delay(1000);
}
void avanzar(){
  digitalWrite(5,0);
  digitalWrite(9,255);
  digitalWrite(10,255);
  digitalWrite(11,0);
}
void retroceder(){
  digitalWrite(5,255);
  digitalWrite(9,0);
  digitalWrite(10,0);
  digitalWrite(11,255);
}

void derecha(){
  digitalWrite(5,255);
  digitalWrite(9,0);
  digitalWrite(10,255);
  digitalWrite(11,0);
}

void izquierda(){
  digitalWrite(5,0);
  digitalWrite(9,255);
  digitalWrite(10,0);
  digitalWrite(11,255);
}

void detener(){
  digitalWrite(5,0);
  digitalWrite(9,0);
  digitalWrite(10,0);
  digitalWrite(11,0);
}

void mvuelta(){
  digitalWrite(5,255);
  digitalWrite(9,0);
  digitalWrite(10,255);
  digitalWrite(11,0);
}
