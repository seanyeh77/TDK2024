#include "header.h"

// 夾爪
class Gripper
{
public:
    Gripper(int pinAngler, int pinClaw, int echo_pin, int trig_pin, int dirPin, int stepPin, int enPin); // 設定角度、爪子伺服、轉盤馬達的腳位

    void setOpen();               // 開啟夾爪
    void setClose();              // 關閉夾爪
    void setClawAngle(int angle); // 設定爪子範圍

    void setUp();                   // 抬起夾爪
    void setDown();                 // 下彎夾爪
    void setAnglerAngle(int angle); // 設定角度
    void addStep();

    void setAnglerRange(int high, int low); // 設定角度範圍
    void setClawPoint(int open, int close); // 設定爪子範圍

    void updateAnglerAngle(); // 設置角度
    void updateClawState();   // 設置爪子狀態

    int getAnglerAngle(); // 獲取當前角度
    int getClawState();   // 獲取當前爪子狀態
    int getDistance();    // 獲取當前與障礙物的距離

    void testAngler(); // 測試角度伺服
    void testClaw();   // 測試爪子伺服

private:
    Servo servoAngler; // 角度伺服
    Servo servoClaw;   // 爪子伺服
    Ultrasonic midUltrasonic;
    int dirPin;
    int stepPin;
    int enPin;

    int high = 140; // 最大角度
    int low = 0;    // 最小角度
    int open = 40;  // 開啟角度
    int close = 10; // 關閉角度

    int targetAnglerAngle; // 當前角度
    bool targetClawState;  // 當前爪子狀態
};

Gripper::Gripper(int pinAngler, int pinClaw, int echo_pin, int trig_pin, int dirPin, int stepPin, int enPin) : midUltrasonic(trig_pin, echo_pin)
{
    servoAngler.attach(pinAngler); // 將角度伺服連接到指定的腳位
    servoClaw.attach(pinClaw);     // 將爪子伺服連接到指定的腳位
    this->dirPin = dirPin;
    this->stepPin = stepPin;
    this->enPin = enPin;
    pinMode(dirPin, OUTPUT);
    pinMode(stepPin, OUTPUT);
    // pinMode(enPin, OUTPUT);
    // digitalWrite(enPin, LOW);
    this->targetAnglerAngle = 0;                                       // 初始化角度
    this->targetClawState = false;                                     // 初始化爪子狀態
    servoAngler.write(this->targetAnglerAngle);                        // 將角度伺服設置到初始角度
    servoClaw.write(this->targetClawState ? this->open : this->close); // 將爪子伺服設置到初始角度
}

// 開啟夾爪
void Gripper::setOpen()
{
    this->targetClawState = true; // 設定角度伺服180度(開啟位置)
}

// 關閉夾爪
void Gripper::setClose()
{
    this->targetClawState = false; // 假設 0 度是開啟的位置
}

void Gripper::setClawAngle(int angle)
{
    this->servoClaw.write(angle);
}

void Gripper::setUp()
{
    this->targetAnglerAngle = high; // 假設 0 度是開啟的位置
}

void Gripper::setDown()
{
    this->targetAnglerAngle = low; // 假設 0 度是開啟的位置
}

void Gripper::setAnglerAngle(int angle)
{
    this->servoAngler.write(angle);
    Serial.println(angle);
}

void Gripper::addStep()
{

    digitalWrite(dirPin, HIGH);
    for (int i = 0; i < 260; i++)
    {
        digitalWrite(stepPin, HIGH);
        delayMicroseconds(500);
        digitalWrite(stepPin, LOW);
        delayMicroseconds(500);
    }
    delay(1000);
}

void Gripper::setAnglerRange(int high, int low)
{
    this->high = high;
    this->low = low;
}

void Gripper::setClawPoint(int open, int close)
{
    this->open = open;
    this->close = close;
}

// 設置的角度
void Gripper::updateAnglerAngle()
{
    if (this->targetAnglerAngle >= 0 && this->targetAnglerAngle <= 180)
    {                                                     // 確保角度在有效範圍內
        this->servoAngler.write(this->targetAnglerAngle); // 設置角度伺服的角度
    }
}

// 設置角度伺服的角度
void Gripper::updateClawState()
{
    this->servoClaw.write(this->targetClawState ? this->open : this->close); // 設置爪子伺服的角度
}

// 獲取當前的角度
int Gripper::getAnglerAngle()
{
    return this->targetAnglerAngle;
}

// 獲取當前的爪子狀態
int Gripper::getClawState()
{
    return this->targetClawState;
}

int Gripper::getDistance()
{
    return this->midUltrasonic.read();
}

// 測試角度伺服
void Gripper::testAngler()
{
    for (int i = this->low; i <= this->high; i++)
    {
        this->servoAngler.write(i);
        delay(10);
    }
    for (int i = this->high; i >= this->low; i--)
    {
        this->servoAngler.write(i);
        delay(10);
    }
}

// 測試爪子伺服
void Gripper::testClaw()
{
    this->servoClaw.write(this->open);
    delay(1000);
    this->servoClaw.write(this->close);
    delay(1000);
}