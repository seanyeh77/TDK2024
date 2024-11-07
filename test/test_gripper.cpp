#include "header.h"
#include "gripper.h"
#include <unity.h>

int PingripperAnglerPin = 31;
int PingripperClawPin = 30;

Gripper* gripper;

void setup() {
    gripper = new Gripper(PingripperAnglerPin, PingripperClawPin);
    Serial.begin(9600);
}
void testGripperClawClose() {
    gripper->testClaw();
}

void loop() {
    UNITY_BEGIN();
    RUN_TEST(testGripperClawClose);
    UNITY_END();
}
