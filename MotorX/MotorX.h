#pragma once
#include <Arduino.h>

#if defined(ESP8266) || defined(ARDUINO_ESP8266_NODEMCU)
#define D0 16
#define D1 5 //  I2C
#define D2 4 //  I2C
#define D3 0
#define D4 2
#define D5 14
#define D6 12
#define D7 13
#define D8 15
#define D9 3  //RX0
#define D10 1 //TX0
#define D12 10
#endif

#if defined(__AVR_ATmega328P__)
#define D0 0 //
#define D1 1
#define D2 2
#define D3 3
#define D4 4
#define D5 5
#define D6 6
#define D7 7
#define D8 8
#define D9 9
#define D10 10
#define D11 11
#define D12 12
#endif

#define L9110 0
#define TA6586 0
#define VHN2SP30 1
#define ON 1
#define OFF 0
#define AUTO 2

/** 
 * @brief Управление мотороми
 * @tparam Типы драйверов для коллекторных мотором TA6586, L9110, VNH2SP30
 * 
 * 
 */

class MotorX
{
public:
#if defined(ESP8266) || defined(ARDUINO_ESP8266_NODEMCU) || defined(__AVR_ATmega328P__)
    void begin(byte chip, byte In1, byte In2, byte InPwm = 254);
#endif
#if defined(ESP32)
    void begin(byte chip, byte In1, byte canal_in1, byte In2, byte canal_in2, byte InPwm = 254, byte canal_InPwm = 254);
#endif

    void On(byte dir_in, byte pwm, byte inc = 254);
    void SvetInit(byte fara_mode, byte port_fara_forvard = 254, int time_On = 30, byte port_fara_back = 254);
    byte fara_forvard = OFF; // ON  OFF  AUTO
    int speed_curent;

private:
    void WriteMotor(byte pwm1, byte pwm2);
    byte dr_chip = 0;
    byte port_in1 = 0;
    byte port_in2 = 0;
    byte port_pwm = 254;
    byte canal1;
    byte canal2;
    int speed = 0; // текущая скорость вращения мотора в формате шим сигнала
    byte dir = 0;  // текущее направление вращения

    byte p_fara_forvard = 254;  // порт подключения передних фар
    byte p_fara_back = 254;     // порт подключения задних фар
    unsigned long t_on = 30;    // время горения фары при простое
    unsigned long t_curent = 0; // время простоя двигателя
    byte b_dir = 1;             // сигнал направления при движении назад
};

/**
 * @brief Класс управления сервоприводами на цифровых портах
 * 
 */
class ServoX
{
public:
    void Attach(byte port, bool revers = true);
    void Write(byte t = 254, byte inc = 0);
    byte Read();
    void On();
    void Off();
    bool ReadMode();

private:
    byte ugol = 90;
    bool mode = true;
    byte port = 0;
    byte c_ugol = 90;
    bool rev = true;
};
float AkbRead(byte port_akb = 255);
