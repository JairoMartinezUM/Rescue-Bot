//Por favor no modifiquen este archivo
#define TRIGGER_PIN  8
#define ECHO_PIN     7

void setup() {
  Serial.begin(9600);
  pinMode(TRIGGER_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(6, OUTPUT); // Pin de control del motor 1
  pinMode(9, OUTPUT); // Pin de control del motor 2
  pinMode(10, OUTPUT); // Pin de control del motor 3
  pinMode(11, OUTPUT); // Pin de control del motor 4
  delay(5000);
}

void loop() {
  long duracion;
  int distancia;
  int valorLinea = SensorIA0(); // Leer el valor del sensor de línea
  
  digitalWrite(TRIGGER_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIGGER_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGGER_PIN, LOW);

  duracion = pulseIn(ECHO_PIN, HIGH);
  distancia = duracion * 0.034 / 2;

  Serial.print("Distancia: ");
  Serial.print(distancia);
  Serial.println(" cm");
  Serial.println(valorLinea);
  
  if (valorLinea > 1 && valorLinea< 200) { // Si detecta una línea, retroceder
    girarRotaciones(1);
  } else if (distancia <20) { // Si detecta un objeto cerca, avanzar con toda potencia
    avanzar();
  } else { // Si no detecta línea ni objeto cerca, girar
    girarLoco();
  }
  delay(50);
}
void retrocederRotaciones(int rotaciones) {
  int pasos = 400*rotaciones;
  
  // Retroceder la cantidad de pasos calculada
  for(int i = 0; i < pasos; i++) {
    retroceder();
    delay(1); // Retardo entre cada paso, ajusta según sea necesario
  }
}
void girarRotaciones(int rotaciones) {
  int pasos = 700*rotaciones;
  
  // Retroceder la cantidad de pasos calculada
  for(int i = 0; i < pasos; i++) {
    girarUno();
    delay(1); // Retardo entre cada paso, ajusta según sea necesario
  }
}

int SensorIA0() {
  int x;
  x = analogRead(A0);
  return x;
}
void avanzar() {
  analogWrite(6, 0);
  analogWrite(9, 255);
  analogWrite(10, 255);
  analogWrite(11, 0);
}
void girarUno(){
  analogWrite(6, 255);
  analogWrite(9, 0);
  analogWrite(10, 0);
  analogWrite(11, 0);
}
void girar() {
  analogWrite(6, 255);
  analogWrite(9, 0);
  analogWrite(10, 255);
  analogWrite(11, 0);
}
void girarLoco(){
  analogWrite(6, 0);
  analogWrite(9, 140);
  analogWrite(10, 120);
  analogWrite(11, 0);
}
void retroceder(){
  analogWrite(6, 150);
  analogWrite(9, 0);
  analogWrite(10, 0);
  analogWrite(11, 150);
}
