/**
 *  Arquivo que definirá a classe de PID com seus cálculos
 */

#pragma once

// Classe com os métodos e atributos relacionados ao cálculo do controlador PID
class PID {

    // Declaração dos atributos privados
    private:

        // Atributos com os parâmetros do controlador PID
        double _Kp = 0; // Ganho proporcional
        double _Ki = 0; // Ganho integral
        double _Kd = 0; // Ganho derivativo

        // Atributos para armazenar o resultado dos termos do algoritmo PID
        double _proportional = 0.; // Ação proporcional
        double _integral     = 0.; // Ação integral
        double _derivative   = 0.; // Ação derivativa

        // Atributo que armazena o erro do último cálculo
        double _lastError = 0.;

    public:

    // gets das constantes
    double getKp(){ return _Kp;}
    double getKd(){ return _Kd;}
    double getKi(){ return _Ki;}

    /**
     * @brief Método que atualiza os parâmetros do controle PID.
     * 
     * @param newKp Novo valor do Kp;
     * @param newKi Novo valor do Ki;
     * @param newKd Novo valor do Kd.
     */
    void updateConstants(double newKp, double newKi, double newKd) {
        // Atualiza os atributos com os valores passados
       _Kp = newKp;
       _Ki = newKi;
       _Kd = newKd;
    }

    /**
     * @brief Método que realiza os cálculos do algoritmo PID
     * 
     * @param error Erro a ser zerado.
     * 
     * @return Resultado do cálculo do PID.
     */
    double calculate(double error) {
        // Calcula os termos do algoritmo PID
       _proportional = _Kp * error;
       _integral += _Ki * error;
       _derivative = _Kd * (error - _lastError);

        // Atualiza o atributo que armazena o erro anterior com o valor do erro atual
       _lastError = error;

        // Retorna a soma dos termos do algoritmo PID
        return _proportional + _integral + _derivative;
    }

    /**
     * @brief Método que reseta os atributos utilizados nos cálculos
     */
    void reset() {
       _integral = 0.;
       _lastError = 0.;
    }
};
