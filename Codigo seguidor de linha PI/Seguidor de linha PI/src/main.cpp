#include <iostream>
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
states currentState = states::stopped;
modes currentMode = modes::undefinedMode;

void PIDfunc();
void getMovement(String command, int *linearSpd, int *angularSpd, int *enable);
void RCfunc();


void setup(){
  Serial.begin(9600);
  
  sensores = new Sensors();
  motors = new Motors();
  pid = new PID();
  interface = new Interface();

  //inicializacoes necessarias
  motors->initMotors();
  sensores->initPins();
  pinMode(BUTTON, INPUT);
  interface->waitStartSignal();

  pid->updateConstants(45,0,950); // dando um valor inicial
  // interface->menuPrompt(pid, motors, &currentState); // printa o menu de opcoes no terminal bluetooth
  interface->SerialBT.println("Conectado!");
}


void loop() {
  if( currentMode == modes:: pidMode){
    Serial.println("PID");
    PIDfunc();
  }
  else if (currentMode == modes::RC){
    // Serial.println("RC");
    RCfunc();
  }
  else if (currentMode == modes::undefinedMode){
    Serial.println("undefined");
    interface->menuInit(&currentMode);
  }
}

void PIDfunc(){

  //bloco para o comando bluetooth
  if (interface->SerialBT.available()){ // verifica se recebemos algo por bluetooth
    interface->menuPrompt(pid, motors,&currentState); // reprinta as opcoes de menu
    interface->menuActions(pid, sensores, motors, &currentState, &currentMode);
  }
  
  //bloco para calibrar sensores
  if (currentState == states::calibrating){
    interface->SerialBT.println("\n\n-->Calibrando Array de Sensores");

    sensores->resetCalibration();
    sensores->calibrateSensors();
    currentState = states::stopped;

    interface->SerialBT.println("\n\n-->Array de Sensores Calibrados!\n");
    interface->menuPrompt(pid, motors,&currentState); // reprinta as opcoes de menu
  }
  if(currentState == states::running){
    sensores->readCalibrated(); // le sensores levando em conta a calibracao
    PIDerror = sensores->calculatePosition(); // pegua erro em relacao a linha
    PIDresult = pid->calculate(PIDerror); // calcula o PID
    motors->moveRobot(PIDresult);
  } 
  else if (currentState == states::stopped){
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

void getMovement(String command, int *linearSpd, int *angularSpd, int *enable){
  // Split the readString by a pre-defined delimiter in a simple way. '%'(percentage) is defined as the delimiter in this project.
  int delimiter1 = command.indexOf(",");
  int delimiter2 = command.indexOf(",", delimiter1 + 1);

  // Define variables to be executed on the code later by collecting information from the readString as substrings.
  String enableStr = command.substring(0, delimiter1);
  String linearStr = command.substring(delimiter1 + 1, delimiter2);
  String angularStr = command.substring(delimiter2 + 1, command.length());

  *linearSpd = linearStr.toInt();
  *angularSpd = angularStr.toInt();
  *enable = enableStr.toInt();
}

void RCfunc(){
  static String command;
  static int linearSpd, angularSpd, enable;

  if (interface->SerialBT.available()){ // verifica se recebemos algo por bluetooth
    command = interface->read();
    getMovement(command, &linearSpd, &angularSpd, &enable);
    Serial.println("RC | Linear Speed: " + String(linearSpd) + " | Angular Speed: " + String(angularSpd) + " | enable: " + String(enable));

    if (enable == 0){ 
      currentMode = modes::undefinedMode;
      return;
    }
   
   motors->setStdSpeed(linearSpd);
   motors->moveRobot(angularSpd);
  }
}