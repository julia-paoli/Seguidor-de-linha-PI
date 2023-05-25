#include <Arduino.h>
#include <Motores/motores.hpp>
#include <Sensores/sensores.hpp>
#include <PID/PID.hpp>

Motors motors;
Sensors sensores;
PID pid;

int PIDresult;
int stdSpeed;
bool calibrating;
double PIDerror;
bool started = false;

void setup() {
  motors.initMotors();
  sensores.initPins();
  pinMode(LED, OUTPUT);
  pinMode(BUTTON, INPUT);
}

void loop() {
  //bloco para calibrar sensores
  if (calibrating){
    digitalWrite(LED,HIGH);
    sensores.resetCalibration();
    sensores.calibrateSensors();
    digitalWrite(LED,LOW);
    calibrating = false; 
  }

  if(started){
    sensores.readCalibrated(); // le sensores levando em conta a calibracao
    PIDerror = sensores.calculatePosition(); // pegua erro em relacao a linha
    PIDresult = pid.calculate(PIDerror); // calcula o PID
    motors.moveRobot(stdSpeed, PIDresult);
  } 
  else{
    motors.stopRobot();
  }

  if(digitalRead(BUTTON)){
    started = !started;
  }

}


