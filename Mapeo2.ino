#include "Adafruit_TCS34725.h"
#include <ServoTimer2.h>

//  pines
#define TRIGGER_PIN  8
#define ECHO_PIN 7
#define MOTOR_A_1 6
#define MOTOR_A_2 9
#define MOTOR_B_1 10
#define MOTOR_B_2 11

// Inicializar el sensor de color TCS34725
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_1X);
ServoTimer2 servoMotorU;
ServoTimer2 servoMotorO;

//  mapa y pila
const int filas = 21;
const int columnas = 11;
char mapa[filas][columnas];
const int MAX_SIZE = 100;
int stackX[MAX_SIZE];
int stackY[MAX_SIZE];
int top = -1;

// Sensores IR
#define IR_SENSOR_1 A0
#define IR_SENSOR_2 A1
#define IR_SENSOR_3 A2
#define IR_SENSOR_4 A3
const int umbralIR = 500;

// Variables de posición inicial
int posX = 15;
int posY = 15;

// Variables para distancias
long distanciaIzq = 0, distanciaCen = 0, distanciaDer = 0;

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

// Funciones de movimiento del robot
void avanzar() {
    analogWrite(MOTOR_A_1, 170);  // Motor A adelante
    analogWrite(MOTOR_A_2, 0);
    analogWrite(MOTOR_B_1, 0);  // Motor B adelante
    analogWrite(MOTOR_B_2, 120);
}

void detener() {
    analogWrite(MOTOR_A_1, 0);
    analogWrite(MOTOR_A_2, 0);
    analogWrite(MOTOR_B_1, 0);
    analogWrite(MOTOR_B_2, 0);
}

void derecha() {
    analogWrite(MOTOR_A_1, 0);
    analogWrite(MOTOR_A_2, 170);
    analogWrite(MOTOR_B_1, 120);
    analogWrite(MOTOR_B_2, 0);
}

void izquierda() {
    analogWrite(MOTOR_A_1, 120);
    analogWrite(MOTOR_A_2, 0);
    analogWrite(MOTOR_B_1, 120);
    analogWrite(MOTOR_B_2, 0);
}

void retroceder() {
    analogWrite(MOTOR_A_1, 255);
    analogWrite(MOTOR_A_2, 0);
    analogWrite(MOTOR_B_1, 0);
    analogWrite(MOTOR_B_2, 255);
}

void escanearParedes() {
    int angulos[] = {1500, 750, 2550};
    int umbralDistancia = 8;

    for (int i = 0; i < 3; i++) {
        switch (i) {
            case 0:
                servoMotorU.write(angulos[0]);
                delay(500);
                distanciaCen = distanciaCalculada();
                break;
            case 1:
                servoMotorU.write(angulos[1]);
                delay(500);
                distanciaDer = distanciaCalculada();
                break;
            case 2:
                servoMotorU.write(angulos[2]);
                delay(500);
                distanciaIzq = distanciaCalculada();
                break;
        }

        if (distanciaCen >= umbralDistancia) {
            avanzar();
        } else if ((distanciaDer > distanciaIzq) && (distanciaDer > distanciaCen) && (distanciaDer >= umbralDistancia)) {
            derecha();
        } else if ((distanciaIzq > distanciaDer) && (distanciaIzq > distanciaCen) && (distanciaIzq >= umbralDistancia)) {
            izquierda();
        } else {
            detener();
        }
        Serial.print("Distancia en derecha: ");
        Serial.print(distanciaDer);
        Serial.print(" cm");
        Serial.println("");
        Serial.print("Distancia en centro: ");
        Serial.print(distanciaCen);
        Serial.print(" cm");
        Serial.println("");
        Serial.print("Distancia en izquierda: ");
        Serial.print(distanciaIzq);
        Serial.print(" cm");
        Serial.println("");
    }
    servoMotorU.write(1500);  // Regresa al centro
}

void setup() {
    Serial.begin(9600);
    pinMode(IR_SENSOR_1, INPUT);
    pinMode(IR_SENSOR_2, INPUT);
    pinMode(IR_SENSOR_3, INPUT);
    pinMode(IR_SENSOR_4, INPUT);

    // Inicializar motores
    pinMode(MOTOR_A_1, OUTPUT);
    pinMode(MOTOR_A_2, OUTPUT);
    pinMode(MOTOR_B_1, OUTPUT);
    pinMode(MOTOR_B_2, OUTPUT);

    // Inicializar el sensor de color
    if (tcs.begin()) {
        Serial.println("Sensor TCS34725 inicializado con éxito.");
    } else {
        Serial.println("No se pudo encontrar el sensor TCS34725.");
    }

    // Inicializar el sensor ultrasonido
    pinMode(TRIGGER_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);
    servoMotorU.attach(3);
    servoMotorU.write(1500);

    // Inicialización del mapa
    for (int i = 0; i < filas; i++) {
        for (int j = 0; j < columnas; j++) {
            mapa[i][j] = ' ';
        }
    }
    mapa[posY][posX] = '0';
    push(posX, posY);
}

void loop() {
    // Leer color detectado y actualizar el mapa
    String colorDetectado = detectarColor();
    if (colorDetectado == "verde") {
        mapa[posY][posX] = '0';
        push(posX, posY);
    } else if (colorDetectado == "rojo") {
        mapa[posY][posX] = 'M';
    }

    // Avanzar el robot
    avanzar();
    delay(2500);
    detener();

    // Control de servos (pinza cerrada)
    servoMotorO.write(1500);
    Serial.println("Pinza cerrada");

    // Distancia ultrasonido y actualización de mapa
    long distancia = distanciaCalculada();
    if (distancia < 5) {
        if (posX >= 0 && posX < columnas && posY >= 0 && posY < filas) {
            mapa[posY][posX] = '|';
        }
    }

    // Realizar escaneo de paredes
    escanearParedes();
    delay(2000); // Espera antes del siguiente escaneo
}