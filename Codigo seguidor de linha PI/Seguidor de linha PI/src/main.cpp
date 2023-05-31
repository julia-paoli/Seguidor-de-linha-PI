#include <Arduino.h>
#include <interface/Interface.hpp>

int PIDresult;
double PIDerror;
bool started = false;

void setup() {
  motors.initMotors();
  sensores.initPins();
  //pinMode(LED, OUTPUT);
  pinMode(BUTTON, INPUT);
}

void loop() {
  //bloco para calibrar sensores
  if (sensores.getCalibrating()){
    //digitalWrite(LED,HIGH);
    sensores.resetCalibration();
    sensores.calibrateSensors();
    //digitalWrite(LED,LOW);
    sensores.setCalibrating(false); 
  }

  if(started){
    sensores.readCalibrated(); // le sensores levando em conta a calibracao
    PIDerror = sensores.calculatePosition(); // pegua erro em relacao a linha
    PIDresult = pid.calculate(PIDerror); // calcula o PID
    motors.moveRobot(PIDresult);
  } 
  else{
    motors.stopRobot();
  }

  if(digitalRead(BUTTON)){
    started = !started;
  }

}


