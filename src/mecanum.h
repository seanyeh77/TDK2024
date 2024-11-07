#include "header.h"

// 計算麥克納姆輪的速度
void calculateMecanumWheelSpeeds(int vx, int vy, int omega, int &frontLeft, int &frontRight, int &backLeft, int &backRight)
{
    // 計算每個輪子的速度
    frontLeft = vx + vy + omega;
    frontRight = vx - vy - omega;
    backLeft = vx - vy + omega;
    backRight = vx + vy - omega;

    // 找出最大和最小的速度
    int maxSpeed = max(frontLeft, max(frontRight, max(backLeft, backRight)));
    int minSpeed = min(frontLeft, min(frontRight, min(backLeft, backRight)));

    // 如果超出範圍，進行線性變換
    if (maxSpeed > 500 || minSpeed < -500)
    {
        float scale = 1.0;
        if (maxSpeed > 500)
        {
            scale = 500.0 / maxSpeed;
        }
        else if (minSpeed < -500)
        {
            scale = -500.0 / minSpeed;
        }

        frontLeft = static_cast<int>(frontLeft * scale);
        frontRight = static_cast<int>(frontRight * scale);
        backLeft = static_cast<int>(backLeft * scale);
        backRight = static_cast<int>(backRight * scale);
    }
}
class Mecanum
{
public:
    Mecanum(int pinFrontLeft, int pinFrontRight, int pinBackLeft, int pinBackRight); // 設定輪子的腳位

    void setMotorSpeeds(int frontLeft, int frontRight, int backLeft, int backRight); // 設定輪子的速度
private:
    Servo motorFrontLeft;  // 前左輪
    Servo motorFrontRight; // 前右輪
    Servo motorBackLeft;   // 後左輪
    Servo motorBackRight;  // 後右輪
};

Mecanum::Mecanum(int pinFrontLeft, int pinFrontRight, int pinBackLeft, int pinBackRight)
{
    motorFrontLeft.attach(pinFrontLeft);   // 將前左輪連接到指定的腳位
    motorFrontRight.attach(pinFrontRight); // 將前右輪連接到指定的腳位
    motorBackLeft.attach(pinBackLeft);     // 將後左輪連接到指定的腳位
    motorBackRight.attach(pinBackRight);   // 將後右輪連接到指定的腳位
}

void Mecanum::setMotorSpeeds(int frontLeft, int frontRight, int backLeft, int backRight)
{
    // 設置每個輪子的速度
    this->motorFrontLeft.writeMicroseconds(frontLeft);
    this->motorFrontRight.writeMicroseconds(frontRight);
    this->motorBackLeft.writeMicroseconds(backLeft);
    this->motorBackRight.writeMicroseconds(backRight);
}