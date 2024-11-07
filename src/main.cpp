#include "header.h"
#include "gripper.h"
#include "elevator.h"
#include "shaft.h"
#include "pump.h"
#include "mecanum.h"
 
void readJoystick();
bool isJoystick(int value, int target, int range = 100);
 
const int PinFrontLeft = 24;
const int PinFrontRight = 25;
const int PinBackLeft = 22;
const int PinBackRight = 26;
const int PingripperAnglerPin = 31;
const int PingripperClawPin = 30;
const int PinGripperEcho = 10;
const int PinGripperTrig = 11;
const int PinElevatorMotor = 18;
const int PinElevatorEcho = 12;
const int PinElevatorTrig = 13;
const int PinElevatorHighSenser = 15;
const int PinElevatorLowSenser = 16;
const int PinBackHighSenser = 50;
const int PinFrontHighSenser = 51;
const int PinShaftSenser = 53;
const int PinShaftMotor = 27;
const int PinPump = 6;
const int PinPumpAngleServo = 7;
const int stepPin = 5;
const int dirPin = 2;
const int enPin = 8;
const int relayBackA = 42;
const int relayBackB = 43;
const int relayBackMotorA = 46;
const int relayBackMotorB = 47;
const int relayFrontA = 41;
const int relayFrontB = 40;
 
Gripper *gripper;
Elevator *elevator;
Shaft *shaft;
Pump *pump;
Mecanum *mecanum;
 
bfs::SbusRx sbus_rx(&Serial2);
bfs::SbusTx sbus_tx(&Serial2);
bfs::SbusData data;
int chs[16];
int testMicosecendMid = 0; // analog中間值(0為未設定)
 
int ballCount = 0;    // 紀錄撿到的球數
int gripperState = 0; // 自動撿球狀態
int Gripperbuttonstate = 0;
 
void setup()
{
    Serial.begin(9600);
    gripper = new Gripper(PingripperAnglerPin, PingripperClawPin, PinGripperEcho, PinGripperTrig, dirPin, stepPin, enPin);    // 夾爪
    elevator = new Elevator(PinElevatorMotor, PinElevatorEcho, PinElevatorTrig, PinElevatorHighSenser, PinElevatorLowSenser); // 升降
    shaft = new Shaft(PinShaftSenser, PinShaftMotor);                                                                         // 轉軸
    pump = new Pump(PinPump, PinPumpAngleServo);                                                                              // 幫浦
    mecanum = new Mecanum(PinFrontLeft, PinFrontRight, PinBackLeft, PinBackRight);                                            // 麥克納姆輪
    pinMode(relayBackA, OUTPUT);
    pinMode(relayBackB, OUTPUT);
    pinMode(relayBackMotorA, OUTPUT);
    pinMode(relayBackMotorB, OUTPUT);
    pinMode(relayFrontA, OUTPUT);
    pinMode(relayFrontB, OUTPUT);
 
    while (!Serial)
    {
    }
    sbus_rx.Begin();
    sbus_tx.Begin();
 
    delay(3000);
}
void loop()
{
    readJoystick(); // 讀取遙控器數據
 
    // print區
    //  Serial.println(chs[11]);
    //  Serial.println(shaft->getPosition());
    //  Serial.println(elevator->getDistance());
 
    // 移動
    int frontLeft, frontRight, backLeft, backRight;
    // 計算麥克納姆輪的速度
    calculateMecanumWheelSpeeds(chs[0] - 1500, chs[1] - 1500, chs[2] - 1500, frontLeft, frontRight, backLeft, backRight);
    // // 限制速度
    // if (isJoystick(2000, chs[4]))
    // {
    //     frontLeft *= 0.3;
    //     frontRight *= 0.3;
    //     backLeft *= 0.3;
    //     backRight *= 0.3;
    // }
    mecanum->setMotorSpeeds(frontLeft + 1500, frontRight + 1500, backLeft + 1500, backRight + 1500);
 
    if (isJoystick(2000, chs[3]))
    {
        digitalWrite(relayBackMotorA, 1);
        digitalWrite(relayBackMotorB, 0);
    }
 
    else if (isJoystick(1000, chs[3]))
    {
        digitalWrite(relayBackMotorA, 0);
        digitalWrite(relayBackMotorB, 1);
    }
 
    else
    {
        digitalWrite(relayBackMotorA, 0);
        digitalWrite(relayBackMotorB, 0);
    }
 
    // // 車上取球
    // if (isJoystick(1000, chs[4]))
    // {
    //     int turnPosition = 765; // 轉軸目標值
    //     int tagetDistance = 24; // 升降目標值
    //     shaft->setShaftAngle(turnPosition);
    //     if (abs(shaft->getPosition() - turnPosition) < 2)
    //     {
    //         shaft->setShaftMicoSeconds(1500);
    //         elevator->setDistance(tagetDistance);
    //     }
    // }
 
    // 手動轉軸
    shaft->setShaftMicoSeconds(chs[5]);
    // if (isJoystick(1000, chs[5]))
    // {
    //     shaft->setShaftMicoSeconds(shaft->upMicroseconds);
    // }
 
    // else if (isJoystick(2000, chs[5]))
    // {
    //     shaft->setShaftMicoSeconds(shaft->downMicroseconds);
    // }
 
    // else
    // {
    //     shaft->setShaftMicoSeconds(1500);
    // }
 
    // 手動伸縮
    if (isJoystick(2000, chs[10]))
    {
        elevator->setDown();
    }
    else if (isJoystick(1000, chs[10]))
    {
        elevator->setUpper();
    }
    else if (isJoystick(1500, chs[10]))
    {
        elevator->setMotorMicoSeconds(1500);
    }
 
    // chs6
    if (isJoystick(1000, chs[6]))
    {
        digitalWrite(relayFrontA, 1);
        digitalWrite(relayFrontB, 0);
    }
 
    else if (isJoystick(2000, chs[6]))
    {
        digitalWrite(relayFrontA, 0);
        digitalWrite(relayFrontB, 1);
    }
 
    else if (PinElevatorHighSenser == HIGH || (1500, chs[6]))
    {
        digitalWrite(relayFrontA, 0);
        digitalWrite(relayFrontB, 0);
    }
 
    // 幫浦角度
    pump->setPumpAngle(map(chs[7], 1000, 2000, 0, 180));
 
    // 幫浦啟動或關閉
    if (isJoystick(2000, chs[8]))
    {
        pump->setPumpState(1);
    }
    else
    {
        pump->setPumpState(0);
    }
 
    // 自動撿球
    if (isJoystick(2000, chs[9]))
    {
        if ((Gripperbuttonstate == 0))
        {
            int distance = gripper->getDistance();                    // 獲取當前與目標物的距離
            if (ballCount>=9){
                    gripper->addStep();
                    Gripperbuttonstate = 1;
            }
            else if (gripperState == 0 && (distance < 5 && distance != 0)) // 偵測到球 -> 閉爪
            {
                if (ballCount <= 6)
                {
                    gripper->setClose();
                    gripper->updateClawState();
                    delay(200);
                    gripperState = 1;
                }
                else if (ballCount == 7)
                {
                    gripper->setClose();
                    gripper->updateClawState();
                    ballCount += 1;
                    Gripperbuttonstate = 1;
                }
                else if (ballCount == 8)
                {
                    gripper->setUp();
                    gripper->updateAnglerAngle();
                    delay(700);
                    gripper->setOpen(); // 開爪
                    gripper->updateClawState();
                    delay(1000);
                    ballCount += 1;
                    Gripperbuttonstate = 1;
                }
                else
                {
                    // gripper->addStep();
                    Gripperbuttonstate = 1;
                }
            }
            else
            {
                switch (gripperState)
                {
                case 1:
                    gripper->setUp(); // 上升
                    gripper->updateAnglerAngle();
                    delay(700);
                    gripperState = 2;
                    break;
 
                case 2:
                    gripper->setOpen(); // 開爪
                    gripper->updateClawState();
                    delay(1000);
                    gripperState = 3;
                    break;
 
                case 3:
                    ballCount++;        // 紀錄撿到的球數
                    gripper->setDown(); // 下降
                    gripper->updateAnglerAngle();
                    delay(500);
                    if (ballCount < 6 || ballCount >= 10)
                    {
                        gripper->addStep();
                    }
                    gripperState = 0;
                    Gripperbuttonstate = 1;
                    break;
                default:
                    gripperState = 0; // 預設狀態
                    gripper->setOpen();
                    gripper->setDown();
                    Gripperbuttonstate = 1;
                    break;
                }
            }
        }
    }
    else
    {
        Gripperbuttonstate = 0;
        gripperState = 0;
        if (ballCount != 8)
        {
            gripper->setOpen();
            gripper->setDown();
        }
        else
        {
            gripper->setClose();
        }
 
        // 向前運動
    }
    Serial.println(ballCount);
    // 後面 車體升降
    if (isJoystick(1000, chs[11]))
    {
        digitalWrite(relayBackA, 1);
        digitalWrite(relayBackB, 0);
    }
 
    else if (isJoystick(2000, chs[11]))
    {
        digitalWrite(relayBackA, 0);
        digitalWrite(relayBackB, 1);
    }
 
    else if (PinElevatorHighSenser == HIGH || (1500, chs[11]))
    {
        digitalWrite(relayBackA, 0);
        digitalWrite(relayBackB, 0);
    }
 
    // else if (isJoystick(2000, chs[10]))
    // {
    //     // mode1
    //     if (isJoystick(1000, chs[4]))
    //     {
    //         int mode1_targethigh = 20;
    //         int mode1_targetAngle = 772;
    //         elevator->setDistance(mode1_targethigh);
    //         if (abs(elevator->getDistance()-mode1_targethigh) < 2)
    //         {
    //             elevator->setDistance(1500);
    //             shaft->setShaftAngle(mode1_targetAngle);
    //         }
    //     }
    //     // mode2
    //     else if (isJoystick(1500, chs[4]))
    //     {
    //         int mode2_targethigh = 14;
    //         int mode2_targetAngle = 790;
    //         elevator->setDistance(mode2_targethigh);
    //         if (abs(elevator->getDistance()-mode2_targethigh) < 2)
    //         {
    //             elevator->setDistance(1500);
    //             shaft->setShaftAngle(mode2_targetAngle);
    //         }
    //     }
    //     // mode3
    //     else if (isJoystick(2000, chs[4]))
    //     {
    //         int mode3_targethigh = 4;
    //         int mode3_targetAngle = 810;
    //         elevator->setDistance(mode3_targethigh);
    //         if (abs(elevator->getDistance()-mode3_targethigh) < 2)
    //         {
    //             elevator->setDistance(1500);
    //             shaft->setShaftAngle(mode3_targetAngle);
    //         }
    //     }
    // }
 
    // Serial.println(elevator->getDistance()); //  輸出當前升降機的高度
 
    // Serial.println(shaft->getPosition());
    // 測試升降+轉軸
    // if (isJoystick(2000, chs[7])) // ch6為高
    // {
    //     // 伸縮
    //     if (isJoystick(1000, chs[6]))
    //     {
    //         testMicosecendMid = testMicosecendMid == 0 ? chs[9] : testMicosecendMid; // 設定ch10為analog中間值
    //         elevator->setMotorMicoSeconds(chs[9] - testMicosecendMid + 1500);
    //         shaft->setShaftMicoSeconds(1500);
    //     }
    //     // 轉軸
    //     else if (isJoystick(2000, chs[6]))
    //     {
    //         testMicosecendMid = testMicosecendMid == 0 ? chs[9] : testMicosecendMid; // 設定ch10為analog中間值
    //         elevator->setMotorMicoSeconds(1500);
    //         shaft->setShaftMicoSeconds(chs[9] - testMicosecendMid + 1500);
    //     }
    //     else
    //     {
    //         testMicosecendMid = 0; // 重設中間值
    //         elevator->setMotorMicoSeconds(1500);
    //         shaft->setShaftMicoSeconds(1500);
    //     }
    // }
    // else
    // {
    //     testMicosecendMid = 0;
    //     elevator->setMotorMicoSeconds(1500);
    //     shaft->setShaftMicoSeconds(1500);
    // }
 
    gripper->updateAnglerAngle();
    gripper->updateClawState();
    elevator->updateMotorMicoSeconds();
    shaft->updateShaftMicoSeconds();
    pump->updatePumpAngle();
 
    delay(10);
}
 
void readJoystick()
{
    if (sbus_rx.Read())
    {
        data = sbus_rx.data();
        for (int i = 0; i < 16; i++)
        {
            chs[i] = data.ch[i];
            chs[i] = map(data.ch[i], 175, 1900, 1000, 2000);
        }
 
        sbus_tx.data(data);
        sbus_tx.Write();
    }
}
 
// 判定遙控器腳位狀態
bool isJoystick(int value, int target, int range)
{
    return abs(value - target) < range;
}
 
 