/*
    Funções referentes a locomoção do robô. Como a função analogwrite é instável
   no ESP32 é necessário usar a função ledcWrite assim como definir os canais.
*/

#pragma once
#include <Pinos/pinos.hpp>

using namespace pins;


/*
https://www.filipeflop.com/blog/motor-dc-arduino-ponte-h-l298n/

Logica da ponte H. Nos que sao HIGH, escrevemosos PWM

                IN1      IN2
HORARIO          1        0
ANTI-HORARIO     0        1
FREIO            1        1
PONTO MORTO      0        0

*/

// Frequência em Hz do sinal de pwm
const int PWMFrequencySignal = 1000;

// Quantidade de bits que define o intervalo (mín - máx) do valor de PWM
const int resolution = 8;


class Motors{
    private:
        int _stdSpeed;

    public:

        /**
         * @brief Funcao para iniciar os canais do robo
         *
         */
        void initMotors() {
            // configuração dos canais, recebendo como parâmetro o canal, a frequência e
            // a resolucao respectivamente
            ledcSetup(channel_DRV_LEFT_IN1, PWMFrequencySignal, resolution);
            ledcSetup(channel_DRV_LEFT_IN2, PWMFrequencySignal, resolution);
            ledcSetup(channel_DRV_RIGHT_IN1, PWMFrequencySignal, resolution);
            ledcSetup(channel_DRV_RIGHT_IN2, PWMFrequencySignal, resolution);

            // Anexação dos canais que geram o pwm aos pinos
            ledcAttachPin(DRV_LEFT_IN1, channel_DRV_LEFT_IN1);
            ledcAttachPin(DRV_LEFT_IN2, channel_DRV_LEFT_IN2);
            ledcAttachPin(DRV_RIGHT_IN1, channel_DRV_RIGHT_IN1);
            ledcAttachPin(DRV_RIGHT_IN2, channel_DRV_RIGHT_IN2);
        }

        /**
         * @brief Funcao simples que para o robo
         *
         */
        void stopRobot() {
            ledcWrite(channel_DRV_LEFT_IN2, 255);
            ledcWrite(channel_DRV_LEFT_IN1, 255);
            ledcWrite(channel_DRV_RIGHT_IN2, 255);
            ledcWrite(channel_DRV_RIGHT_IN1, 255);
        }

        /**
         * @brief Manda PWM para os motores fazendo os ajustes de mixagem necessários.
         * Essa função toma como conhecida a noção de que :
         * PWM_direita = vel_linear + vel_angular
         * PWM_esquerda = vel_linear - vel_angular
         * 
         * @param left
         * @param right
         */
        void setPWMs(int left, int right) {

            // Vamos ver se os valores escolhidos estão dentro da resolução do ESP
            int difPos;
            int difNeg;

            if (max(left, right) > 255) {
                // Se um dos valores passar de 255, precisamos retirar o "excedente" de ambos
                // isso é necessário para preservar a diferença entre eles e, consequentemente, preservar a velocidade angular desejada

                difPos = max(left, right) - 255;
                left -= difPos;
                right -= difPos;
            }
            if (min(left, right) < -255) {
                // mesma coisa mas cuidando do limite inferior

                difNeg = min(left, right) + 255; 
                left -= difNeg;
                right -= difNeg;
            }

            //Agora sabemos que ambos os PWMs estão dentro dos limites de resolução e que a velocidade angular foi conservada

            // Lógica da Ponte H -----------------------------------------------

            // motores espelhados-> provavelmente relacionado ao modo q os drivers foram conectados  (ver isso)
            
            // Motor esquerdo
            if (left > 0) {
                // motor esquerdo roda no anti-horario
                ledcWrite(channel_DRV_LEFT_IN1, 0);
                ledcWrite(channel_DRV_LEFT_IN2, left);

            }
            else {
                // motor esquerdo roda no sentido horario
                ledcWrite(channel_DRV_LEFT_IN1, abs(left));
                ledcWrite(channel_DRV_LEFT_IN2, 0);
            }

            //Motor direito
            if (right > 0) {
                // motor direito roda no sentido horario
                ledcWrite(channel_DRV_RIGHT_IN1, right);
                ledcWrite(channel_DRV_RIGHT_IN2, 0);

            } else {
                // motor direito roda no sentido anti horario
                ledcWrite(channel_DRV_RIGHT_IN1, 0);
                ledcWrite(channel_DRV_RIGHT_IN2, abs(right));
            }
        }


        /**
         * @brief Função para realizar o movimento do robô aplicando o pid 
         * 
         */
        void moveRobot(int angularDiference){

        int right, left;

        // Cálculo da velocidade em cada motor
        right = _stdSpeed;
        right += angularDiference;

        left = _stdSpeed;
        left -= angularDiference;
        // Tradução de velocidade em PWM não é igual, portanto deve ser alterado
        this->setPWMs (left, right);
        }

        void setStdSpeed(int newStdSpeed){
            _stdSpeed = newStdSpeed;
        }
};


Motors motors;
