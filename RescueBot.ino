#define TRIGGER_PIN 8
#define ECHO_PIN    7
#include "Adafruit_TCS34725.h"
#include <Servo.h>;
#include <Stack.h>;
#include <Array.h>;

Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_1X);
Servo servoMotorU;
Servo servoMotorO;

const int filas = 21, columnas = 11;
char mapa[filas][columnas];
std::stack<std::array<int,2>> pila; //guardar coordenadas x,y NO ESTA FUNCIONANDO

int posX = 7;
int posY = 20;

void setup() {
  Serial.begin(9600);
  pinMode(TRIGGER_PIN, OUTPUT); //Ultrasonico
  pinMode(ECHO_PIN, INPUT); //Ultrasonico
  pinMode(6, OUTPUT); //Motor Derecho -
  pinMode(9, OUTPUT); //Motor Derecho +
  pinMode(10, OUTPUT); //Motor Izquierdo +
  pinMode(11, OUTPUT); //Motor Izquierdo -
  servoMotorU.attach(6); //Servo del Ultrasonico
  servoMotorO.attach(7); //Servo de la pinza para el objeto
  pinMode(A0, INPUT); //IR 1
  pinMode(A1, INPUT); //IR 2
  pinMode(A2, INPUT); //IR 3
  pinMode(A3, INPUT); //IR 4

  for(int i=0; i<filas; i++){
    for(int j=0; j<columnas; j++){
      mapa[filas][columnas]= ' ';
    }
  }

  mapa[posY][posX] = '0';
  pila.push({posY,posX});//NO ESTA FUNCIONANDO
  
  tcs.begin();// Sensor de Color
}

void loop() {
  // put your main code here, to run repeatedly:
  int filas = 21, columnas = 11;
  char mapa[filas][columnas] = {};
  //origen = mapa[21][7]
  
  String colorDetectado = detectarColor();
  if (colorDetectado == "verde"){
    Serial.println("verde");
    mapa[21][7] = '0'; //origen
    //añadir a pila
    pila.push({21,7});//NO ESTA FUNCIONANDO
  }else if(colorDetectado == "rojo"){
    //mapa[][] = '0'; //meta
    Serial.println("rojo");
  }else{
    Serial.println("no valido");
  }

  //detectar pared y añadirlo a mapa
  //habra un rango para el registro
  long distancia = distanciaCalculada();
  if(distancia < 10){
    mapa[][] = '#';//NENCESITAMOS DECIRLE DONDE GUARDARLO
  }

delay(1000);
}

void movimientoIR(){
  //servoU de ultrasonico
  //A0,A1,A2,A3
  int statusA0 = digitalRead(A0);
  int statusA1 = digitalRead(A1);
  int statusA2 = digitalRead(A2);
  int statusA3 = digitalRead(A3);

  Serial.println(String(statusA0) + String(statusA1) + String(statusA2)+ String(statusA3));
  
  if(statusA0==1 && statusA1==0 && statusA2==0 && statusA3==1){
    avanzar();
  }if(statusA0==0 && statusA1==0 && statusA2==0 && statusA3==0){
    detener();
  }
  
}

long distanciaCalculada(){
  digitalWrite(TRIGGER_PIN,LOW);
  delayMicroseconds(2);
  digitalWrite(TRIGGER_PIN,HIGH);
  delayMicroseconds(2);
  digitalWrite(TRIGGER_PIN,LOW);

  long duracion = pulseIn(ECHO_PIN,HIGH);
  long distancia = (duracion * 0.034)/2;
  return distancia;
}

String detectarColor() {
  uint16_t r, g, b, c;
  tcs.getRawData(&r, &g, &b, &c);

  Serial.print("R: "); Serial.print(r);
  Serial.print(" G: "); Serial.print(g);
  Serial.print(" B: "); Serial.print(b);
  Serial.print(" C: "); Serial.println(c);

  // Determinar el color basado en valores RGB
  if (c > 500) { // Umbral de brillo general
    if (g > r && g > b) {
      return "verde";
    } else if (r > g && r > b) {
      return "rojo";
    }
  }
  return "ninguno"; // No se detectó ni verde ni rojo
}

void avanzar(){
  digitalWrite(6,255);
  digitalWrite(9,0);
  digitalWrite(10,0);
  digitalWrite(11,255);
}

void derecha(){
  digitalWrite(6,0);
  digitalWrite(9,255);
  digitalWrite(10,0);
  digitalWrite(11,255);
  delay(1);
}

void izquierda(){
  digitalWrite(6,255);
  digitalWrite(9,0);
  digitalWrite(10,255);
  digitalWrite(11,0);
  delay(1);
}

void detener(){
  digitalWrite(6,0);
  digitalWrite(9,0);
  digitalWrite(10,0);
  digitalWrite(11,0);
}

void mvuelta(){
  digitalWrite(6,0);
  digitalWrite(9,255);
  digitalWrite(10,0);
  digitalWrite(11,255);
  delay (2);
}
