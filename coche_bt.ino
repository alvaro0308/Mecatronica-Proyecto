#include <AccelStepper.h>
#include <Thread.h>

#define FULLSTEP 4
//Definicion pins de los motores
const int motor1_Pin1 = 6; //Motor1 28BYJ48 In1
const int motor1_Pin2 = 7; //Motor1 28BYJ48 In2
const int motor1_Pin3 = 8; //Motor1 28BYJ48 In3
const int motor1_Pin4 = 9; //Motor1 28BYJ48 In4
const int motor2_Pin1 = 10; //Motor2 28BYJ48 In1
const int motor2_Pin2 = 11; //Motor2 28BYJ48 In2
const int motor2_Pin3 = 12; //Motor2 28BYJ48 In3
const int motor2_Pin4 = 13; //Motor2 28BYJ48 In4
//Definicion variables motor
AccelStepper stepper1(FULLSTEP, motor1_Pin1, motor1_Pin3, motor1_Pin2, motor1_Pin4);
AccelStepper stepper2(FULLSTEP, motor2_Pin1, motor2_Pin3, motor2_Pin2, motor2_Pin4);

//Definicion pins ultrasonido
const int trigger = 3; //Envio
const int echo = 2; //Recepcion
//Definicion variables ultrasonido
long duracion; //Tiempo que tarda el ultrasonido en recibirse
int distancia = 40; //Distancia del obstaculo al sensor

int modo_bt = 's'; //Inicio en stop.
int detectado = 0;
unsigned long TiempoAhora = 0;
unsigned long TiempoAhora2 = 0;

Thread ultrasonidos_thread = Thread();

void callback_ultrasonidos(){
  //Mandamos un pulso de sonido durante 10 microsegundos
  digitalWrite(trigger, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigger, LOW);
  duracion = pulseIn(echo, HIGH); //Medimos el tiempo que ha tardado en llegar el sonido
  distancia = (duracion * 0.034) / 2; //Calculamos la distancia multiplicando por la velocidad del sonido y diviendo entre 2
}

void setup(){ 
  Serial.begin(9600); //Inicio del puerto serial para comunicacion con el bluetooth.

  stepper1.setMaxSpeed(2000);
  stepper2.setMaxSpeed(2000);

  //Declarar pines como entrada/salida
  pinMode(trigger, OUTPUT);
  pinMode(echo, INPUT);

  ultrasonidos_thread.onRun(callback_ultrasonidos);
  ultrasonidos_thread.setInterval(1000);
} 

void loop(){ 
  if(Serial.available()>0){ //Si puede, lee lo que recibe por bluetooth.
    modo_bt = Serial.read();
  }
  
  if(modo_bt == 'f'){ //Forward.
    stepper1.setSpeed(500);
    stepper2.setSpeed(-500);
    stepper1.runSpeed();
    stepper2.runSpeed();   
  }
  if(modo_bt == 'l'){ //Left.
    stepper1.setSpeed(500);
    stepper1.runSpeed();     
  }
  if(modo_bt == 's'){ //Stop. 
  }
  if(modo_bt == 'r'){ //Right.
    stepper2.setSpeed(-500);
    stepper2.runSpeed();
  } 
  if(modo_bt == 'b'){ //Backward.
    stepper1.setSpeed(-500);
    stepper2.setSpeed(500);
    stepper1.runSpeed();
    stepper2.runSpeed();
  }
  if (modo_bt == 'a'){ //Auto.
    if(ultrasonidos_thread.shouldRun()){
      ultrasonidos_thread.run();
    }
    
    if(distancia <= 10 && distancia >= 2 || detectado){
      detectado = 1;
      TiempoAhora = millis();
      while(millis() < TiempoAhora+3000){
        stepper1.setSpeed(-500);
        stepper2.setSpeed(500);
        stepper1.runSpeed();
        stepper2.runSpeed();
      }
      TiempoAhora2 = millis();
      while(millis() < TiempoAhora2+3000){
        stepper2.setSpeed(-500);
        stepper2.runSpeed();
        detectado = 0;
      }
    }
    else{
      stepper1.setSpeed(500);
      stepper2.setSpeed(-500);
      stepper1.runSpeed();
      stepper2.runSpeed();
    }
  }
}
