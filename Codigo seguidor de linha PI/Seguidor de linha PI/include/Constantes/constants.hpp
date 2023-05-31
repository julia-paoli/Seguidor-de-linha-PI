/*
    Cabeçalho com a definição dos pinos que são usados no robôs.
*/

#pragma once
#include <Arduino.h>

namespace pins {

    // Definicao dos pinos destinados aos sensores
    const int LEFT_SENSOR4_PIN      = 36;
    const int LEFT_SENSOR3_PIN      = 39;
    const int LEFT_SENSOR2_PIN      = 34;
    const int LEFT_SENSOR1_PIN      = 35;
    const int RIGHT_SENSOR1_PIN     = 32;
    const int RIGHT_SENSOR2_PIN     = 33;
    const int RIGHT_SENSOR3_PIN     = 25;
    const int RIGHT_SENSOR4_PIN     = 26;

    // Definicao do codigo dos sensores (endereco na lista de sensores)
    const int LEFT_SENSOR4  = 0;
    const int LEFT_SENSOR3  = 1;
    const int LEFT_SENSOR2  = 2;
    const int LEFT_SENSOR1  = 3;
    const int RIGHT_SENSOR1 = 4;
    const int RIGHT_SENSOR2 = 5;
    const int RIGHT_SENSOR3 = 6;
    const int RIGHT_SENSOR4 = 7;


    // Quantidade de sensores dispostos na frente e laterais
    const int SENSORS_NUMBER = 8;
   

    // Definição dos pinos dos drivers de motor
    const int DRV_LEFT_IN1 = 19;
    const int DRV_LEFT_IN2 = 18;
    const int DRV_RIGHT_IN1 = 17;
    const int DRV_RIGHT_IN2 = 16;

    // Canais responsáveis pelo sinal pwm
    const int channel_DRV_LEFT_IN1 = 0;
    const int channel_DRV_LEFT_IN2 = 1;
    const int channel_DRV_RIGHT_IN1 = 2;
    const int channel_DRV_RIGHT_IN2 = 3;

    // Definição dos pinos de LED
    //const int LED = 2; //provisorio
    const int BUTTON = 2;

}  // namespace pins

const int MAX_ESP = 4095;
const int LINE_VALUE = MAX_ESP*0.9;