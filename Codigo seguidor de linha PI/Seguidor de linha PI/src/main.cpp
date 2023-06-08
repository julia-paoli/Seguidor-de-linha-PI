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
states currentState = stopped;


void setup(){
  
  sensores = new Sensors();
  motors = new Motors();
  pid = new PID();
  interface = new Interface();

  //inicializacoes necessarias
  motors->initMotors();
  sensores->initPins();
  pinMode(BUTTON, INPUT);
  interface->waitStartSignal();
  interface->menuPrompt(pid, motors,&currentState); // printa o menu de opcoes no terminal bluetooth
  //pinMode(LED, OUTPUT);

  pid->updateConstants(7,0,100); // dando um valor inicial
  Serial.begin(9600);
}


void loop() {

  //bloco para o comando bluetooth
  if (interface->SerialBT.available()){ // verifica se recebemos algo por bluetooth
    interface->menuActions(pid, motors,&currentState);
    interface->menuPrompt(pid, motors,&currentState); // reprinta as opcoes de menu
  }
  
  //bloco para calibrar sensores
  if (currentState == states::calibrating){
    //digitalWrite(LED,HIGH);

    //prints de debugg
    //Serial.println("CALIBRANDO  ");

    sensores->resetCalibration();
    sensores->calibrateSensors();
    //digitalWrite(LED,LOW);
    currentState = states::stopped;
  }
  if(currentState = states::running){
    //prints de debugg
    //Serial.println("RODANDO  ");
    //Serial.println(String(motors->getStdSpeed()) + "  |   " + String(pid.getKp()) + "  |   " + String(pid.getKi()) + "  |   " + String(pid.getKd()));
   
    sensores->readCalibrated(); // le sensores levando em conta a calibracao
    PIDerror = sensores->calculatePosition(); // pegua erro em relacao a linha
    PIDresult = pid->calculate(PIDerror); // calcula o PID
    motors->moveRobot(PIDresult);
  } 
  else if (currentState = states::stopped){
    //prints de debugg
    //Serial.println("PARADO");
    motors->stopRobot();
  }

  // "troca" de estados
  if(digitalRead(BUTTON)){
    //parando se nao estiver parado e se estiver parado coloca em running
    if(currentState == states::stopped){
      currentState = states::running;
    }
    else{
      currentState = states::stopped;
    }
    delay(300);
  }

}


