#include <BluetoothSerial.h>
#include "Motores/motores.hpp"
#include "PID/PID.hpp"
#include "Sensores/sensores.hpp"


/**
 * @brief opcoes do menu
 * 
 */
enum menuOptions{
    stdSpeed,
    calibrating,
    kp,
    ki,
    kd,
    turnOn,
    turnOff
};

enum states{running, calibrating, stopped};

class Interface{
    public:
        Interface();
        BluetoothSerial SerialBT;
        String read();

        void menuPrompt();
        void menuActions(PID* pid, Motors* motors, Sensors* sensors,states*);
        void ChangeStdSpeed(Motors*);
        void ChangePIDconstants(menuOptions, PID*);
        void ChangeState(states* state, states newstate);
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

void Interface::ChangeState(states* state, states newstate){
    this->SerialBT.println("");
    this->SerialBT.println("--> Digite 1 para confirmar sua escolha: ");
    this->SerialBT.println("--> Caso ao contrario, aperte qualquer outra tecla: ");
    String confirmation = this->read(); // lendo confirmacao
    this->SerialBT.println("");

   if(confirmation.toInt()== 1){
    *state = newstate;
   }
}

/**
 * @brief Mostra as opcoes para o usuario e chama as funcoes adequadas para alteracao das variaveis
 * 
 */
void Interface::menuPrompt(){
    this->SerialBT.println("");
    this->SerialBT.println("--> Digite a opcao desejada ");
    this->SerialBT.println("");
    this->SerialBT.println("--> 1 Alterar a velocidade linear ");
    this->SerialBT.println("--> 2 Colocar em modo de calibracao ");
    this->SerialBT.println("--> 3 Alterar o Kp do PID ");
    this->SerialBT.println("--> 4 Alterar o Ki do PID ");
    this->SerialBT.println("--> 5 Alterar o Kd do PID ");
    this->SerialBT.println("--> 6 Iniciar corrida");
    this->SerialBT.println("--> 7 Parar");
}

void Interface::menuActions(PID* pid, Motors* motors, Sensors* sensors, states* state){
    String option = this->read();

    switch (option.toInt())
    {
        case menuOptions::stdSpeed : {
            this->ChangeStdSpeed(motors);
            break;
        }
        case menuOptions::calibrating:{
             this->ChangeState(state, states::calibrating);
            break;
        }
        case menuOptions::kp:{
            this->ChangePIDconstants(menuOptions::kp,pid);
            break;
        }
        case menuOptions::ki:{
            this->ChangePIDconstants(menuOptions::kp,pid);
            break;
        }
        case menuOptions::kd:{
            this->ChangePIDconstants(menuOptions::kp,pid);
            break;
        }
        case menuOptions::turnOn:{
            this->ChangeState(state, states::running);
        }
         case menuOptions::turnOff:{
              this->ChangeState(state, states::stopped);
        }
        default:
            this->SerialBT.println("--> Selecao invalida, tente de novo ");    
    }
      this->SerialBT.println("");

}
