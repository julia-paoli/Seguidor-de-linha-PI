#include <Arduino.h>
#include <Motores/motores.hpp>
#include <Sensores/sensores.hpp>
#include <PID/PID.hpp>

Motors motors;
Sensors sensores;
int kp,ki,kd;
PID pid(kp,ki,kd);

int PIDresult;
bool calibrating;
double PIDerror;

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

  if(digitalRead(BUTTON)){
    sensores.readCalibrated(); // le sensores levando em conta a calibracao
    PIDerror = sensores.calculatePosition(); // pegua erro em relacao a linha
    PIDresult = pid.calculate(PIDerror); // calcula o PID
    moveRobot(PIDresult); // usa o resultado do PID para movimentar o robo
} 

}


/**
 * @brief Função para realizar o movimento do robô aplicando o pid 
 * 
 */
void moveRobot(int angularDiference){

    int right, left;

    // Cálculo da velocidade em cada motor
    right = motors.stdSpeed;
    right += angularDiference;

    left = motors.stdSpeed;
    left -= angularDiference;
    // Tradução de velocidade em PWM não é igual, portanto deve ser alterado
    motors.setPWMs (left, right);
}