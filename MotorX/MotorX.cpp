#include <MotorX.h>

#if defined(ESP8266) || defined(ARDUINO_ESP8266_NODEMCU) || defined(__AVR_ATmega328P__)

/**
 * @brief Создаем новый мотор 
 * 
 * @param chip тип микросхемы драйвера коллекторного мотора ( L9110=0, TA6586=0, VHN2SP30=1 )
 * @param In1 1 порт входного сигнала для управления скоростью ( PWM )
 * @param In2 2 порт входного сигнала для управления скоростью ( PWM )
 * @param InPwm порт шим сигнала только для микросхемы VHN2SP30
 */
void MotorX::begin(byte chip, byte In1, byte In2, byte InPwm)
{
    dr_chip = chip;
    port_in1 = In1;
    port_in2 = In2;
    port_pwm = InPwm;

    pinMode(In1, OUTPUT);
    pinMode(In2, OUTPUT);
    pinMode(InPwm, OUTPUT);
}
#endif

#if defined(ESP32)
/**
 * @brief Создаем новый мотор 
 * 
 * @param chip тип микросхемы драйвера коллекторного мотора ( L9110=0, TA6586=0, VHN2SP30=1 )
 * @param In1 1 Первый порт входного сигнала для управления скоростью ( PWM )
 * @param canal_in1 Номер первого шим канала только для ESP32
 * @param In2 2 Второй порт входного сигнала для управления скоростью ( PWM )
 * @param canal_in2 Номер второго шим канала только для ESP32
 * @param InPwm порт шим сигнала только для микросхемы VHN2SP30
 * @param canal_InPwm Номер шим канала только для микросхемы VHN2SP30
 */
void MotorX::begin(byte chip, byte In1, byte canal_in1, byte In2, byte canal_in2, byte InPwm, byte canal_InPwm)
{
    dr_chip = chip;
    port_in1 = In1;
    port_in2 = In2;
    port_pwm = InPwm;
    pinMode(In1, OUTPUT);
    pinMode(In2, OUTPUT);
    pinMode(InPwm, OUTPUT);

    if (dr_chip == L9110 || dr_chip == TA6586)
    {
        ledcAttachPin(In1, canal_in1);
        ledcAttachPin(In2, canal_in2);
        ledcSetup(canal_in1, 5000, 8);
        ledcSetup(canal_in2, 5000, 8);
        canal1 = canal_in1;
        canal2 = canal_in2;
    }

    if (dr_chip == VHN2SP30)
    {
        ledcAttachPin(InPwm, canal_InPwm);
        ledcSetup(canal_InPwm, 5000, 8);
        canal1 = canal_InPwm;
    }
}

#endif

/**
 * @brief Запуск вращения двигателя с плавным ускорением или на прямую от pwm
 * 
 * @param dir_in Направление вращения мотора
 * @tparam dir=2 холостой ход
 * @tparam dir=0 направление вперед
 * @tparam dir=1 направление назад
 * @tparam dir=3 тормоз двигателя если потдерживает драйвер
 * 
 * @param pwm Шим сионал - управление скоростью вращения 
 * @param inc ускорение от 0 до pwm (если inc = pwm скорость равна pwm)
 */
void MotorX::On(byte dir_in, byte pwm, byte inc)
{
    // блок управления фарами и стоп сигналом
    //===========================================
    if (p_fara_forvard != 254) // если объявлен порт для фары
    {
        if (fara_forvard == 1) // Режим включить фары
        {
            digitalWrite(p_fara_forvard, 0);
        }
        if (fara_forvard == 0) // Режим выключить фары
        {
            digitalWrite(p_fara_forvard, 1);
        }
        if (fara_forvard == 2) // Режим автоматически выключить фары после стоянки
        {
            if (speed > 0)
            {
                t_curent = millis(); // обнулить счетчик простоя
                digitalWrite(p_fara_forvard, 0);
            }
            else
            {
                if (millis() - t_curent > t_on * 1000) // выключить фары если простаивает заданное время
                {
                    digitalWrite(p_fara_forvard, 1);
                }
            }
        }
    }

    if (speed == 0 && dir_in < 2)
        dir = dir_in;

    if (dir_in != dir)
    {
        pwm = constrain(speed - 1, 0, 254);
    }
    if (inc == pwm)
        speed = pwm;
    else
    {
        speed = speed + (inc * (speed < pwm)) - (inc * (speed > pwm));
    }

    speed = constrain(speed, 0, 254);

    if (dr_chip == L9110 || dr_chip == TA6586)
    {
        if (dir_in == 2)
            WriteMotor(0, 0);
        if (dir_in == 3)
            WriteMotor(254, 254);
        else
            WriteMotor(pwm * dir, pwm * !dir);
    }
    if (dr_chip == VHN2SP30)
    {
        if (dir_in == 2)
        {
            digitalWrite(port_in1, 0);
            digitalWrite(port_in2, 0);
        }
        if (dir_in == 3)
        {
            digitalWrite(port_in1, 1);
            digitalWrite(port_in2, 1);
        }
        else
        {

            //Serial.println(String(dir) + "  " + String(dir_in) + "  " + String(speed) + "  " + String(pwm));
            digitalWrite(port_in1, dir);
            digitalWrite(port_in2, !dir);
#if defined(ESP8266) || defined(ARDUINO_ESP8266_NODEMCU) || defined(__AVR_ATmega328P__)
            analogWrite(port_pwm, speed);
#endif
#if defined(ESP32)
            ledcWrite(canal1, speed);
#endif
        }
    }
    //  управление стоп сигналом

    if (p_fara_back != 254) // если объявлен поррт фары
    {
        // Serial.println(String(speed) + "   " + String(pwm) + "   " + String(dir_in));
        if ((speed > pwm) || dir_in == 3)
        //if (dir_in == 3)
        {
            digitalWrite(p_fara_back, 1); // включить стоп сигнал
        }
        else
        {
            digitalWrite(p_fara_back, 0); // выключить стоп сигнал
        }
    }
    //===========================================
}

void MotorX::WriteMotor(byte pwm1, byte pwm2)
{
#if defined(ESP8266) || defined(ARDUINO_ESP8266_NODEMCU) || defined(__AVR_ATmega328P__)
    analogWrite(port_in1, pwm1);
    analogWrite(port_in2, pwm2);
#endif
#if defined(ESP32)
    ledcWrite(canal1, pwm1);
    ledcWrite(canal2, pwm2);
#endif
}

void MotorX::SvetInit(byte fara_mode, byte port_fara_forvard, int time_On, byte port_fara_back)
{
    if (port_fara_forvard != 254)
        p_fara_forvard = port_fara_forvard;

    if (port_fara_back != 254)
        p_fara_back = port_fara_back;

    fara_forvard = fara_mode;
    t_on = time_On;
    pinMode(p_fara_back, OUTPUT);
    pinMode(p_fara_forvard, OUTPUT);
}

/**
 * @brief Поворот сервопривода на указанный угол
 * 
 * @param t Угол поворота в градусах от 0 до 180
 * @param inc Величина-скорость наращивания угла до указано в параметре "t"
 * @tparam inc Если равен нулю, то угол устанавливается мгновенно до  указанного
 * @tparam  Вызов функции без параметров установит последний установленный угол поворота
 * @tparam Который можно получить командой Servo.Read() 
 */
void ServoX::Write(byte t, byte inc)
{
    if (mode)
    {
        inc = constrain(inc, 0, 180);
        if (t != 254)
        {
            t = constrain(t, 0, 180);
            ugol = t;
        }
        if (rev)
            t = 90 + (ugol - 90);
        else
            t = 90 - (ugol - 90);

        if (inc > 0)
        {
            c_ugol = c_ugol + (inc * (c_ugol < t)) - (inc * (c_ugol > t));
            c_ugol = constrain(c_ugol, 0, 180);
        }
        else
        {
            c_ugol = constrain(t, 0, 180);
        }

        int t_u = map(c_ugol, 0, 180, 540, 2400);

        digitalWrite(port, 1);
        delayMicroseconds(t_u);
        digitalWrite(port, 0);
        delayMicroseconds(20000 - t_u);
    }
}
/**
 * @brief Установка параметров сервопривода
 * 
 * @param p Цифровой порт подключения сервопривода
 * @param revers  TRUE или FALSE смена направления вращения сервопривода
 */
void ServoX::Attach(byte p, bool revers)
{
    port = p;
    mode = true;
    rev = revers;
    pinMode(port, OUTPUT);
}
/**
 * @brief Разрешает работу серво привода
 * 
 */
void ServoX::On()
{
    mode = true;
}
/**
 * @brief Отключает работу серво привода 
 * 
 */
void ServoX::Off()
{
    mode = false;
}
/**
 * @brief Возвращает установленный угол на серво приводе 
 * 
 * @return byte Угол в градусах
 */
byte ServoX::Read()
{
    return ugol;
}
/**
 * @brief Возвращает текущий режим работы серво привода
 * 
 * @retval True Серво привод включен
 * @retval False Серво привод отключен
 */
bool ServoX::ReadMode()
{
    return mode;
}
