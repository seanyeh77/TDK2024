#include "header.h"

// 往上:1400 往下:1600

// 設定操控模式種類
enum class ElevatorMode
{
    PWM,
    Distance,
    limitSwitch
};
enum class LimitMode
{
    Upper,
    Down
};

class Elevator
{
public:
    Elevator(int motor_pin, int echo_pin, int trig_pin, int highSenser_pin, int lowSenser_pin);    // 設定升降的腳位

    void setMotorMicoSeconds(int value);    // 設置馬達PWM值

    void setUpMicroseconds(int value);  // 設置上升時的PWM值

    void setDownMicroseconds(int value);    // 設置下降時的PWM值

    void setUpper(); // 將目標設定為上升距離
    void setDown();  // 將目標設定為下降距離
    void setDistance(int distance); // 將目標設定為自訂距離

    void updateMotorMicoSeconds();  // 更新馬達PWM值

    int getDistance(); // 獲取當前的高度
    int getHighSwitchState(); // 獲取頂部限位開關的狀態
    int getLowSwitchState(); // 獲取底部限位開關的狀態
private:
    Ultrasonic ultrasonic;
    Servo motor;
    int highSenser_pin;
    int lowSenser_pin;

    int motor_Microseconds; // 馬達當前PWM值

    int upMicroseconds = 1600;   // 上升時的PWM值
    int downMicroseconds = 1400; // 下降時的PWM值

    ElevatorMode mode = ElevatorMode::PWM; // 設定運行模式("PWM" or "Distance")
    int targetDistance;                    // 目標距離
    LimitMode currentLimitMode = LimitMode::Upper; // 設定限位模式("High" or "Low")

    int high = 38; // 最大距離(cm)38
    int low = 3;   // 最低距離(cm)3
};

// 升降腳位初始化
Elevator::Elevator(int motor_pin, int echo_pin, int trig_pin, int highSenser_pin, int lowSenser_pin) : ultrasonic(trig_pin, echo_pin)
{
    motor.attach(motor_pin);
    pinMode(highSenser_pin, INPUT_PULLUP);
    pinMode(lowSenser_pin, INPUT_PULLUP);
    this->highSenser_pin = highSenser_pin;
    this->lowSenser_pin = lowSenser_pin;
    this->motor_Microseconds = 1500;
    motor.writeMicroseconds(motor_Microseconds);
}

// 設置馬達PWM值
void Elevator::setMotorMicoSeconds(int value)
{
    this->mode = ElevatorMode::PWM; // 設定運行模式為PWM
    this->motor_Microseconds = value > 2000 ? 2000 : value < 1000 ? 1000
                                                                  : value;
}

// 若運行模式為以距離作為目標時，設定設置上升時的PWM值
void Elevator::setUpMicroseconds(int value)
{
    this->upMicroseconds = value;
}

// 若運行模式為以距離作為目標時，設定設置下降時的PWM值
void Elevator::setDownMicroseconds(int value)
{
    this->downMicroseconds = value;
}

// 設置頂部距離為距離目標
void Elevator::setUpper()
{
    this->mode = ElevatorMode::limitSwitch; // 設定運行模式為以距離作為目標
    this->currentLimitMode = LimitMode::Upper;
}

// 設置底部距離為距離目標
void Elevator::setDown()
{
    this->mode = ElevatorMode::limitSwitch; // 設定運行模式為以距離作為目標
    this->currentLimitMode = LimitMode::Down;
}

// 設置自訂距離為距離目標
void Elevator::setDistance(int distance)
{
    this->mode = ElevatorMode::Distance; // 設定運行模式為以距離作為目標
    this->targetDistance = distance;
}

// 更新馬達PWM值
void Elevator::updateMotorMicoSeconds()
{
    switch (mode)
    {
    case ElevatorMode::PWM: // 若運行模式為PWM，則直接設置PWM值
        // if (((this->getDistance() > this->high||digitalRead(highSenser_pin)==LOW) && (this->motor_Microseconds - 1500) < 0) || ((this->getDistance() < this->low||digitalRead(lowSenser_pin)==LOW) && (this->motor_Microseconds - 1500) > 0))
        if (((digitalRead(highSenser_pin)==LOW) && (this->motor_Microseconds - 1500) > 0) || ((digitalRead(lowSenser_pin)==LOW) && (this->motor_Microseconds - 1500) < 0))
        {
            this->motor.writeMicroseconds(1500);
        }
        else
        {
            this->motor.writeMicroseconds(this->motor_Microseconds);
        }
        break;

    case ElevatorMode::Distance: // 如果當前距離大於等於目標距離，則停止上升，否則繼續上升，直到達到目標距離，同理下降
        if ((digitalRead(highSenser_pin)==LOW)&&(this->getDistance() < this->targetDistance))
        {
            this->motor.writeMicroseconds(1500);
        }
        else if ((digitalRead(lowSenser_pin)==LOW)&&(this->getDistance() >= this->targetDistance))
        {
            this->motor.writeMicroseconds(1500);
        }
        else if (this->getDistance() >= this->targetDistance)
        {
            this->motor.writeMicroseconds(this->downMicroseconds);
        }
        else if (this->getDistance() < this->targetDistance)
        {
            this->motor.writeMicroseconds(this->upMicroseconds);
        }
        else
        {
            this->motor.writeMicroseconds(1500);
        }
        break;
    case ElevatorMode::limitSwitch: // 如果當前距離大於等於頂部距離，則停止上升，否則繼續上升，直到達到頂部距離，同理下降
        if ((digitalRead(highSenser_pin)==LOW)&&this->currentLimitMode == LimitMode::Upper)
        {
            this->motor.writeMicroseconds(1500);
        }
        else if ((digitalRead(lowSenser_pin)==LOW)&&(this->currentLimitMode == LimitMode::Down))
        {
            this->motor.writeMicroseconds(1500);
        }
        else
        {
            this->motor.writeMicroseconds(this->currentLimitMode == LimitMode::Upper ? this->upMicroseconds : this->downMicroseconds);
        }
        break;
    default:
        break;
    }
}

// 獲取升降當前的高度
int Elevator::getDistance()
{
    return ultrasonic.read();
}

// 獲取頂部限位開關的狀態
int Elevator::getHighSwitchState()
{
    return !digitalRead(this->highSenser_pin);
}   

// 獲取底部限位開關的狀態
int Elevator::getLowSwitchState()
{
    return !digitalRead(this->lowSenser_pin);
}