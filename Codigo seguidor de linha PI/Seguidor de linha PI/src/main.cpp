#include <Arduino.h>
#include <interface/Interface.hpp>
#include <Motores/motores.hpp>
#include <Sensores/sensores.hpp>
#include <PID/PID.hpp>

int PIDresult;
double PIDerror;
bool started = false;

Sensors *sensores; 
Motors *motors; 
PID *pid;
Interface *interface;

void setup()                                {
  
  sensores = new Sensors();
  motors = new Motors();
  pid = new PID();
  interface = new Interface();
  motors->initMotors();
  sensores->initPins();
  pinMode(BUTTON, INPUT);
  interface->waitStartSignal();
  //pinMode(LED, OUTPUT);

  pid->updateConstants(7,0,100); // dando um valor inicial
  Serial.begin(9600);
}


void loop() {
  //bloco para calibrar sensores
  if (started && sensores->getCalibrating()){
    //digitalWrite(LED,HIGH);

     //prints de debugg
    Serial.println("CALIBRANDO  ");

    sensores->resetCalibration();
    sensores->calibrateSensors();
    //digitalWrite(LED,LOW);
    sensores->setCalibrating(false); 
  }

  if(started){

    //prints de debugg
    Serial.println("RODANDO  ");
    // Serial.println(String(motors->getStdSpeed()) + "  |   " + String(pid.getKp()) + "  |   " + String(pid.getKi()) + "  |   " + String(pid.getKd()));
   
    sensores->readCalibrated(); // le sensores levando em conta a calibracao
    PIDerror = sensores->calculatePosition(); // pegua erro em relacao a linha
    PIDresult = pid->calculate(PIDerror); // calcula o PID
    delay(100);
    motors->moveRobot(PIDresult);
  } 
  else{
    //prints de debugg
    Serial.println("PARADO");
    interface->menu(pid, motors,sensores,&started);
    motors->stopRobot();
  }

  if(digitalRead(BUTTON)){
    delay(300);
    started = !started;
  }

}


