class Pump
{
public:
    Pump(int pumpPin,int pumpAnglerServoPin);

    void setPumpState(int value);

    void setPumpAngle(int angle);

    void updatePumpAngle();

    int getPosition(); // 獲取當前與障礙物的距離
private:
    Servo Angler;
    int pumpState;
    int pumpPin;
    int pumpAnglerServoPin;
    int pumpAngle;
};

Pump::Pump(int pumpPin,int pumpAnglerServoPin)
{
    this->pumpPin = pumpPin;
    this->pumpAnglerServoPin = pumpAnglerServoPin;
    this->pumpState = 0;
    pinMode(pumpPin, OUTPUT);
    Angler.attach(pumpAnglerServoPin);
    Angler.write(0);
    
}

void Pump::setPumpState(int value)
{
    this->pumpState = value;
    digitalWrite(this->pumpPin, value);
}

void Pump::setPumpAngle(int angle){
    this->pumpAngle = angle;
}

void Pump::updatePumpAngle(){
    Angler.write(this->pumpAngle);
}