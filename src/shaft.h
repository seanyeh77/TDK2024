#include "header.h"
 
enum class ShaftMode
{
    PWM,
    Angle
};
 
// 最高點809 最低點738 往上1600 往下1300 最高點865 最低點780
class Shaft
{
public:
    Shaft(int Shaft_pin, int Motor_pin);
 
    void setShaftMicoSeconds(int value);
 
    void setShaftAngle(int angle);
 
    void updateShaftMicoSeconds();
 
    int getPosition(); // 獲取當前與障礙物的距離
    int upMicroseconds = 1750;
    int downMicroseconds = 1200;
 
private:
    Servo Shaft_Motor;
    int PinSenser;
    int position;
    ShaftMode mode = ShaftMode::PWM;
    int targetAngle;
 
    int high = 894;
    int low = 772;
 
    int Shaft_Microseconds;
};
 
Shaft::Shaft(int PinSenser, int Motor_pin)
{
    pinMode(PinSenser, INPUT);
    this->PinSenser = PinSenser;
 
    Shaft_Motor.attach(Motor_pin);
    this->Shaft_Microseconds = 1500;
    Shaft_Motor.writeMicroseconds(1500);
}
 
void Shaft::setShaftMicoSeconds(int value)
{
    this->mode = ShaftMode::PWM;
    this->Shaft_Microseconds = value > 2000 ? 2000 : value < 1000 ? 1000
                                                                  : value;
}
 
void Shaft::setShaftAngle(int angle)
{
    // this->mode = ShaftMode::Angle;
    // this->targetAngle = angle;
}
 
void Shaft::updateShaftMicoSeconds()
{
    switch (this->mode)
    {
    case ShaftMode::PWM:
        if ((this->getPosition() == 1 && (this->Shaft_Microseconds - 1500) < 0))
        {
            this->Shaft_Motor.writeMicroseconds(1500);
        }
        else
        {
            this->Shaft_Motor.writeMicroseconds(this->Shaft_Microseconds);
        }
        break;
    case ShaftMode::Angle:
        if (this->getPosition() == HIGH)
        {
            this->Shaft_Motor.writeMicroseconds(1500);
        }
        else if (abs(this->getPosition() - this->targetAngle) < 5)
        {
            this->Shaft_Motor.writeMicroseconds(1500 + (this->getPosition() - this->targetAngle) ? (downMicroseconds - 1500) * 0.9 : (upMicroseconds - 1500) * 0.9);
        }
        else if (this->getPosition() > this->targetAngle)
        {
            this->Shaft_Motor.writeMicroseconds(this->downMicroseconds);
        }
        else if (this->getPosition() < this->targetAngle)
        {
            this->Shaft_Motor.writeMicroseconds(this->upMicroseconds);
        }
        break;
    default:
        break;
    }
}
 
int Shaft::getPosition()
{
    return digitalRead(this->PinSenser);
}