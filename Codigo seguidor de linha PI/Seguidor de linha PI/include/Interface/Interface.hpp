#include <BluetoothSerial.h>
#include "Motores/motores.hpp"
#include "PID/PID.hpp"
#include "Sensores/sensores.hpp"

class Interface{
    public:
        Interface();
        BluetoothSerial SerialBT;
        String read();

        void menu();
        void ChangeStdSpeed();
        void ChangePIDconstants(String);
        void ChangeCalibrating();
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
 * @return String  lida
 */
String Interface::read() {
    while (SerialBT.available() == 0);          // Aguarda a receber os dados
    String readStr = SerialBT.readString();
    readStr.trim();                            // arruma a string
    return readStr;
}

/**
 * @brief altera a stdSpeed
 * 
 */
void Interface::ChangeStdSpeed(){
    this->SerialBT.println("");
    this->SerialBT.println("--> Insira o novo valor para velocidade: ");
    String newStdSpeed = this->read(); // lendo o valor colocado
    this->SerialBT.println("Valor inserido foi: " + String(newStdSpeed));
    this->SerialBT.println("");

    motors.setStdSpeed(newStdSpeed.toInt()); // altera valor da stdSpeed
}

/**
 * @brief coloca em modo de calibracao, alterando a variavel calibrating
 * 
 */
void Interface::ChangeCalibrating(){
    this->SerialBT.println("");
    this->SerialBT.println("--> Digite 1 para confirmar que deseja entrar em modo de calibracao: ");
    this->SerialBT.println("--> Caso ao contrario, aperte qualquer outra tecla: ");
    String confirmation = this->read(); // lendo confirmacao
    this->SerialBT.println("");

   if(confirmation.toInt()== 1){
    sensores.setCalibrating(false);
   }
}

/**
 * @brief altera uma ocnstante do PID
 * 
 * @param type string que diz se queremos mudra o kp,ki ou kd
 * 
 * 
 */
void Interface::ChangePIDconstants(String type){
    this->SerialBT.println("");
    this->SerialBT.println("--> Insira o novo valor da constante: ");
    String newK = this->read(); // lendo o valor colocado

    if(type == "kp"){
        pid.updateConstants(newK.toDouble(),pid.getKi(),pid.getKd());
    }
    else if(type == "ki"){
        pid.updateConstants(pid.getKp(),newK.toDouble(),pid.getKd());
    }
    else if(type == "kd"){
        pid.updateConstants(pid.getKp(),pid.getKi(),newK.toDouble());
    }
    this->SerialBT.println("Valor inserido foi: " + String(newK));
    this->SerialBT.println("");
}

/**
 * @brief Mostra as opcoes para o usuario e chama as funcoes adequadas para alteracao das variaveis
 * 
 */
void Interface::menu(){
    this->SerialBT.println("");
    this->SerialBT.println("--> Digite a opcao desejada ");
    this->SerialBT.println("");
    this->SerialBT.println("--> 1 Alterar a velocidade linear ");
    this->SerialBT.println("--> 2 Colocar em modo de calibracao ");
    this->SerialBT.println("--> 3 Alterar o Kp do PID ");
    this->SerialBT.println("--> 4 Alterar o Ki do PID ");
    this->SerialBT.println("--> 5 Alterar o Kd do PID ");
    String option = this->read();

    switch (option.toInt())
    {
        case 1: {
            this->ChangeStdSpeed();
            break;
        }
        case 2:{
            this->ChangeStdSpeed();
            break;
        }
        case 3:{
            this->ChangePIDconstants("kp");
            break;
        }
        case 4:{
            this->ChangePIDconstants("ki");
            break;
        }
        case 5:{
            this->ChangePIDconstants("kd");
            break;
        }
        this->SerialBT.println("--> Selecao invalida, tente de novo ");    
    }
      this->SerialBT.println("");

}

Interface interface;