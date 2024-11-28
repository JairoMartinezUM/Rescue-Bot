  #include <Adafruit_TCS34725.h>
  #include <ServoTimer2.h>
  #define TRIGGER_PIN  3
  #define ECHO_PIN 4
  #define TAMANIO_PILA 30 
  Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_1X);
  ServoTimer2 servoMotorU, servoMotorO;
  const int filas = 25;
  const int columnas = 11;
  char mapa[filas][columnas];
  int pilaX[TAMANIO_PILA], pilaY[TAMANIO_PILA];
  int topeX = -1, topeY = -1, paso = 1;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(TRIGGER_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(6,OUTPUT);
  pinMode(9,OUTPUT);
  pinMode(10,OUTPUT);
  pinMode(11,OUTPUT);
  servoMotorU.attach(6);
  servoMotorO.attach(7);
  //servo pinza
  pinMode(A0,INPUT);//IR 1
  pinMode(A1,INPUT);//2
  pinMode(A2,INPUT);//3
  pinMode(A3,INPUT);//IR 4

  for(int i=0;i<filas;i++){
    for(int j=0;j<columnas;j++)
    mapa[i][j] = '?';
  }

  if (!tcs.begin()) {
    Serial.println("Sensor de color no detectado.");
    while (1); // Detén el programa si falla
}
    Serial.println("Iniciando escaneo :P ...");
}

void resolverLaberinto() {
    int x = 0, y = 0;  // Coordenadas iniciales
    int nuevoX, nuevoY;
    registrarCelda(x, y, paso); // Marcar celda inicial
    pushX(x); pushY(y);      // Guardar en las pilas

    while (topeX >= 0 && topeY >= 0) {
        if (tieneVecinoLibre(x, y, nuevoX, nuevoY)) {
            // Mover al vecino libre
            x = nuevoX;
            y = nuevoY;
            registrarCelda(x, y, paso); // Marcar el paso
            pushX(x);
            pushY(y);
        } else {
            // No hay vecinos libres, retroceder
            retroceder(x, y);
        }
    }
}

int detectarObstaculo(){
  digitalWrite(TRIGGER_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIGGER_PIN, HIGH);
  delayMicroseconds(2);
  digitalWrite(TRIGGER_PIN, LOW);

  long duracion = pulseIn(ECHO_PIN, HIGH);
  int distancia = duracion * 0.034 / 2;

  if(distancia < 15){
    return 1;
  }else{
    return 0;
  }
}

void registrarCelda(int x, int y, int paso) {
    mapa[x][y] = '0' + (paso % 10); // Almacena el paso como un carácter.
}

void moverYEscanear(int x, int y, int direccion){
  int nuevaX = x, nuevaY = y;

  switch(direccion){
    case 0: nuevaX--;break; //ADELANTE
    case 1: nuevaY++;break; //DERECHA
    case 2: nuevaX++;break; //ATRAS
    case 3: nuevaY--;break; //IZQUIERDA
  }

  if(nuevaX>=0 && nuevaX<filas && nuevaY>=0 && nuevaY<columnas){
    int obstaculo = detectarObstaculo();
    registrarCelda(nuevaX, nuevaY, obstaculo);
      if(!obstaculo){
        avanzar();
      }
  }  
}

void imprimirMapa(){
  for(int i=0; i<filas; i++){
    for(int j=0; j<columnas; j++){
      Serial.print(mapa[i][j]);
      Serial.print(" ");
    }
    Serial.println();
  }
}

bool tieneVecinoLibre(int x, int y, int &nuevoX, int &nuevoY) {
    if (x > 0 && mapa[x-1][y] == '0') { // Arriba
        nuevoX = x - 1; nuevoY = y; return true;
    }
    if (y < columnas - 1 && mapa[x][y+1] == '0') { // Derecha
        nuevoX = x; nuevoY = y + 1; return true;
    }
    if (x < filas - 1 && mapa[x+1][y] == '0') { // Abajo
        nuevoX = x + 1; nuevoY = y; return true;
    }
    if (y > 0 && mapa[x][y-1] == '0') { // Izquierda
        nuevoX = x; nuevoY = y - 1; return true;
    }
    return false;
}

void avanzar(){
  digitalWrite(6,0);
  digitalWrite(9,255);
  digitalWrite(10,0);
  digitalWrite(11,255);
}

void derecha(){
  digitalWrite(6,0);
  digitalWrite(9,255);
  digitalWrite(10,0);
  digitalWrite(11,255);
}

void izquierda(){
  digitalWrite(6,255);
  digitalWrite(9,0);
  digitalWrite(10,255);
  digitalWrite(11,0);
}

void detener(){
  digitalWrite(6,0);
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

void retroceder(int &x, int &y) {
    if (topeX >= 0 && topeY >= 0) {
        x = popX(); // Recuperar la posición previa de las pilas.
        y = popY();
    }
}

String analizarColor(){
  tcs.begin();
  uint16_t r,g;
  tcs.getRawData(&r, &g, NULL, NULL);

  if(r>g && r>500){
    return "ROJO";
  }else if(g>r && g>500){
    return "VERDE";
  }else{
    return "NIGG";
  }
}

bool pushX(int dato){
  if(topeX < TAMANIO_PILA){
    pilaX[++topeX] = dato;
    return true;
  }
  return false; //pila llena
}

bool pushY(int dato){
  if(topeY < TAMANIO_PILA){
    pilaY[++topeY] = dato;
    return true;
  }
  return false; //pila llena
}

int popX(){
  if(topeX >= 0){
    return pilaX[topeX--];
  }
  return -1; //pila vacía
}

int popY(){
  if(topeY >= 0){
    return pilaY[topeY--];
  }
  return -1; //pila vacía
}

void loop() {
  // put your main code here, to run repeatedly:
  //moverYEscanear();//ingresar coordenadas
  //imprimirMapa();
  //de//lay(1000);
  avanzar();
  delay(1000);
  derecha();
  delay(1000);
}