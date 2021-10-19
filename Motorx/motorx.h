#pragma once
#include <Arduino.h>

#define L9110 0
#define TA6586 1
#define VHN2SP30 2

/** 
 * @brief Управление мотороми
 * @tparam Типы драйверов для коллекторных мотором TA6586, L9110, VNH2SP30
 * 
 * 
 */

class MotorX
{
public:
    void begin(byte chip, byte In1, byte In2);
    void On();

private:
};
