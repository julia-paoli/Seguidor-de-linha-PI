#include <BluetoothSerial.h>
#include "Motores/motores.hpp"
#include "PID/PID.hpp"
#include "Sensores/sensores.hpp"
#include "Constantes/constants.hpp"

enum initMenuOptions{
    undefinedOp,
    pidOp,
    rcOp
};

/**
 * @brief opcoes do menu
 * 
 */
enum menuOptions{
    turnOn,
    turnOff,
    calibrate,
    stdSpeed,
    kp,
    ki,
    kd,
    debugMotors,
    debugSensors,
    exitPID
};

enum states{running, calibrating, stopped, amount}; 
enum modes{undefinedMode,pidMode, RC, amountMode}; 
String statesStrings[states::amount] = {"correndo","calibrando","parado"};

class Interface{
    public:
        Interface();
        BluetoothSerial SerialBT;
        String read();

        void menuInit(modes* modes);
        void menuPrompt(PID* pid, Motors* motors, states* state);
        void menuActions(PID* pid, Sensors *sensores, Motors* motors,states*,modes*);
        void ChangeStdSpeed(Motors*);
        void ChangePIDconstants(menuOptions, PID*);
        void ChangeState(states* state, states newstate);
        void ChangeMode(modes* mode, modes newMode);
        void DebugMotors(Motors *motors);
        void DebugSensors(Sensors *sensors);
        void waitStartSignal();
    private:
};

/**
 * @brief inicializa a interface
 * 
 */
Interface::Interface() {
  SerialBT.begin("ESP32"); //iniciando
}

/**
 * @brief  recebe, guarda e retorna dados
 * 
 * @return String lida
 */
String Interface::read() {
    while (SerialBT.available() == 0);          // Aguarda a receber os dados
    String readStr = SerialBT.readString();
    // Serial.println("READ: [" + readStr + "]");
    readStr.trim();                            // arruma a string
    return readStr;
}

/**
 * @brief Espera a conexao inicial com o terminal bluetooth
 * 
 */
void Interface::waitStartSignal() 
{
  bool hasntCmd = true;
  do {
      SerialBT.println("Digite '1' pra comecar: ");
      String option = read();
      if (option == "1"){
        hasntCmd = false;
      }
      delay(1000);
  } while (hasntCmd);
  SerialBT.println("Iniciando...");
}

/**
 * @brief altera a stdSpeed
 * 
 */
void Interface::ChangeStdSpeed(Motors *motors){
    this->SerialBT.println("");
    this->SerialBT.println("--> Insira o novo valor para velocidade: ");
    String newStdSpeed = this->read(); // lendo o valor colocado
    this->SerialBT.println("Valor inserido foi: " + String(newStdSpeed));
    this->SerialBT.println("");

    motors->setStdSpeed(newStdSpeed.toInt()); // altera valor da stdSpeed
}


/**
 * @brief altera uma constante do PID
 * 
 * @param type string que diz se queremos mudra o kp,ki ou kd
 * 
 * 
 */
void Interface::ChangePIDconstants(menuOptions type, PID* pid){
    this->SerialBT.println("");
    this->SerialBT.println("--> Insira o novo valor da constante: ");
    String newK = this->read(); // lendo o valor colocado

    if(type == menuOptions::kp){
        pid->updateConstants(newK.toDouble(),pid->getKi(),pid->getKd());
    }
    else if(type == menuOptions::ki){
        pid->updateConstants(pid->getKp(),newK.toDouble(),pid->getKd());
    }
    else if(type == menuOptions::kd){
        pid->updateConstants(pid->getKp(),pid->getKi(),newK.toDouble());
    }
    this->SerialBT.println("Valor inserido foi: " + String(newK));
    this->SerialBT.println("");
}

void Interface::DebugMotors(Motors *motors){
    int speed = 50;
    motors->setPWMs(speed, speed);
    delay(3000);
    motors->setPWMs(-speed, speed);
    delay(3000);
    motors->setPWMs(speed, -speed);
    delay(3000);
    motors->setPWMs(0, 0);
}

void Interface::DebugSensors(Sensors *sensors){
    int *values;
    unsigned long lastTime = millis(); 
    do{
        String msg = "";
        this->SerialBT.print(String(millis()) + " - ");
        values = sensors->getCalibratedValues();
        for (int i=0; i < SENSORS_NUMBER; i++)
            msg += String(values[i]) + " ";
        this->SerialBT.println(msg);
    } while (millis() - lastTime < 10000);
}

void Interface::ChangeState(states* state, states newstate){
    *state = newstate;
}

void Interface::ChangeMode(modes* mode, modes newMode){
    *mode = newMode;
}

void Interface::menuInit(modes* mode){
    String option = this->read();
    long intOption = option.toInt();

    Serial.println("MenuInt: " + String(intOption));

    if (intOption == initMenuOptions::pidOp)
        this->ChangeMode(mode, modes::pidMode);
    else if (intOption == initMenuOptions::rcOp)
        this->ChangeMode(mode, modes::RC);
    else
        this->ChangeMode(mode, modes::undefinedMode);
}

/**
 * @brief Mostra as opcoes para o usuario e chama as funcoes adequadas para alteracao das variaveis
 * 
 */
void Interface::menuPrompt(PID* pid, Motors* motors, states* state){
    this->SerialBT.println("=======================================");
    this->SerialBT.println("=========== MENU DE OPCOES ============");
    this->SerialBT.println("=======================================");
    this->SerialBT.println("0) Entrar em modo running");
    this->SerialBT.println("1) Entrar em modo parado");
    this->SerialBT.println("2) Entrar em modo de calibracao" );
    this->SerialBT.println("3) Alterar a velocidade linear (" + String(motors->getStdSpeed()) + ") " );
    this->SerialBT.println("4) Alterar o Kp (" + String(pid->getKp(),1) + ") do PID " );
    this->SerialBT.println("5) Alterar o Ki (" + String(pid->getKi(),1) + ") do PID " );
    this->SerialBT.println("6) Alterar o Kd (" + String(pid->getKd(),1) + ") do PID " );
    this->SerialBT.println("7) Testar motores");
    this->SerialBT.println("8) Testar sensores" );
    this->SerialBT.println("9) Sair do modo PID" );
    this->SerialBT.println("--> Estado atual : " + statesStrings[*state]);
    this->SerialBT.println("");
    this->SerialBT.print("--> Digite a opcao desejada: ");
}

void Interface::menuActions(PID* pid, Sensors *sensores, Motors* motors, states* state,modes* mode){
    String option = this->read();
    long intOption = option.toInt();

    if (intOption == menuOptions::turnOn)
        this->ChangeState(state, states::running);
    else if (intOption == menuOptions::turnOff)
        this->ChangeState(state, states::stopped);
    else if (intOption == menuOptions::calibrate)
        this->ChangeState(state, states::calibrating);
    else if (intOption == menuOptions::stdSpeed)
        this->ChangeStdSpeed(motors);
    else if (intOption == menuOptions::kp)
        this->ChangePIDconstants(menuOptions::kp,pid);
    else if (intOption == menuOptions::ki)
        this->ChangePIDconstants(menuOptions::ki,pid);
    else if (intOption == menuOptions::kd)
        this->ChangePIDconstants(menuOptions::kd,pid);
    else if (intOption == menuOptions::debugMotors)
        this->DebugMotors(motors);
    else if (intOption == menuOptions::debugSensors)
        this->DebugSensors(sensores);
    else if (intOption == menuOptions::exitPID)
        *mode = modes::undefinedMode;
    else
        this->SerialBT.println("--> Selecao invalida, tente de novo ");    

      this->SerialBT.println("");
}