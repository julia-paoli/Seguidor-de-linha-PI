/*
    Cabeçalho com funções referentes à leitura dos sensores de linha.
*/

#pragma once

#include <Pinos/pinos.hpp>

using namespace pins;

// É criada a classe Sensores, visando organizar as funções que serão utilizadas
// para leitura dos sensores
class Sensors {
   public:

    /**
        * @brief Construtor
        * 
        */
        Sensors() {
            // Define as variáveis para cada sensor
            for (int i = 0; i < SENSORS_NUMBER; i++) {
                _maxCalibrated[i] = 0;
                _minCalibrated[i] = MAX_ESP;
                _readings[i]      = 0;
            }
        }

        /**
         * @brief Inicializa os pinos
         * 
         */
        void initPins() {
            // O sinal dos sensores são recebidos pelos pinos próprios do ESP
            pinMode(LEFT_SENSOR4_PIN, INPUT);
            pinMode(LEFT_SENSOR3_PIN, INPUT);
            pinMode(LEFT_SENSOR2_PIN, INPUT);
            pinMode(LEFT_SENSOR1_PIN, INPUT);
            pinMode(RIGHT_SENSOR1_PIN, INPUT);
            pinMode(RIGHT_SENSOR2_PIN, INPUT);
            pinMode(RIGHT_SENSOR3_PIN, INPUT);
            pinMode(RIGHT_SENSOR4_PIN, INPUT);
        }

        /**
         * @brief Calibra os sensores
         * 
         */
        void calibrateSensors() {
            readSensors();
            // ajustando a calibração para cada sensor
            for (int i = 0; i < SENSORS_NUMBER; i++) {
                if (_maxCalibrated[i] < _readings[i])
                    _maxCalibrated[i] = _readings[i];

                if (_minCalibrated[i] > _readings[i])
                    _minCalibrated[i] = _readings[i];
            }
        }

        /**
         * @brief Reseta a calibracao
         * 
         */
        void resetCalibration() {
            for (int i = 0; i < SENSORS_NUMBER; i++) {
                _maxCalibrated[i] = MAX_ESP;
                _minCalibrated[i] = 0;
            }
        }
        
        /**
         * @brief Le os sensores, ja levando em conta a calibracao
         * 
         */
        void readCalibrated() {
            int readings;

            readSensors();
            for (int i = 0; i < SENSORS_NUMBER ; i++) {
                readings = map(_readings[i], _minCalibrated[i], _maxCalibrated[i], 0,
                            MAX_ESP);
                readings = constrain(readings, 0, MAX_ESP);
                _readings[i] = readings;
            }
        
        }


        /**
        * @brief Calcula a posição do robô na linha de percurso de acordo com as
        * informações dos sensores frontais
        */
        double calculatePosition() {
        // Sensores fontrais = sensores totais - sensores laterais
        float        result     = 100;
        static float lastResult = 0;

        // Leitura dos sensores de linha na ordem da esquerda para direita
        int readingsInOrder[SENSORS_NUMBER ] = {_readings[LEFT_SENSOR4],
                                        _readings[LEFT_SENSOR3],
                                        _readings[LEFT_SENSOR2],
                                        _readings[LEFT_SENSOR1],
                                        _readings[RIGHT_SENSOR1],
                                        _readings[RIGHT_SENSOR2],
                                        _readings[RIGHT_SENSOR3],
                                        _readings[RIGHT_SENSOR4]};

        int   index      = 0;
        int casesNumber = 21;
        // Combinações possíveis de ativação dos sensores, outras possibilidades não são possíveis para a lógica.
        int   cases[casesNumber]  = {1,   12, 123, 2,   23, 234, 3,   34, 345, 4, 45, 456, 5,  56,  567, 6,  67,  678, 7,  78,  8};
        // Pesos associados a cada combinação de sensores
        // Para mais de um sensor sendo lido, é feita a média de pesos entre os sensores
        float weight[casesNumber] = {4, 3.5, 3, 3, 2.5, 2, 2, 1.5, 1, 1, 0,-1,  -1,  -1.5, -2,  -2, -2.5, -3,  -3,  -3.5, -4};

        for (int i = 1; i <= SENSORS_NUMBER; i++) {
            if (readingsInOrder[i - 1] <= LINE_VALUE) {
                index *= 10;
                index += i;
            }
        }

        for (int i = 0; i < casesNumber; i++) {
            if (cases[i] == index) {
                result = weight[i];
            }
        }
        if (result == 100) {
            result = lastResult;
        }

        lastResult = result;

        return result;
        }

   private:

        // Vetores onde são guardadas as leituras dos sensores
        int _readings[SENSORS_NUMBER];
        int _maxCalibrated[SENSORS_NUMBER];
        int _minCalibrated[SENSORS_NUMBER];
        
        /**
         * @brief Le o valor nos sensores sem a calibracao
         * 
         */
        void readSensors() {
            // le cada sensor e altera o valor da variavel global leituras
            _readings[LEFT_SENSOR4]  = analogRead(LEFT_SENSOR4_PIN);
            _readings[LEFT_SENSOR3]  = analogRead(LEFT_SENSOR3_PIN);
            _readings[LEFT_SENSOR2]  = analogRead(LEFT_SENSOR2_PIN);
            _readings[LEFT_SENSOR1]  = analogRead(LEFT_SENSOR1_PIN);
            _readings[RIGHT_SENSOR1] = analogRead(RIGHT_SENSOR1_PIN);
            _readings[RIGHT_SENSOR2] = analogRead(RIGHT_SENSOR2_PIN);
            _readings[RIGHT_SENSOR3] = analogRead(RIGHT_SENSOR3_PIN);
            _readings[RIGHT_SENSOR4] = analogRead(RIGHT_SENSOR4_PIN);

        }

    
};