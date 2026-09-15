const int leftMotorPin1 = 22;
const int leftMotorPin2 = 23;
const int rightMotorPin1 = 24;
const int rightMotorPin2 = 25;

const int sensorLeftPin = 30;
const int sensorRightPin = 32;

void setup() {
    pinMode(leftMotorPin1, OUTPUT);
    pinMode(leftMotorPin2, OUTPUT);
    pinMode(rightMotorPin1, OUTPUT);
    pinMode(rightMotorPin2, OUTPUT);

    pinMode(sensorLeftPin, INPUT);
    pinMode(sensorRightPin, INPUT);

    stopMotors();
}

void loop() {
    bool isLeftOnBlack = (digitalRead(sensorLeftPin) == HIGH);
    bool isRightOnBlack = (digitalRead(sensorRightPin) == HIGH);

    if (!isLeftOnBlack && !isRightOnBlack) {
        goForward();
    }
    else if (isLeftOnBlack && !isRightOnBlack) {
        turnLeftGently();
    }
    else if (!isLeftOnBlack && isRightOnBlack) {
        turnRightGently();
    }
    else {
        stopMotors();
    }
}

void goForward() {
    digitalWrite(leftMotorPin1, HIGH);
    digitalWrite(leftMotorPin2, LOW);
    digitalWrite(rightMotorPin1, HIGH);
    digitalWrite(rightMotorPin2, LOW);
}

void turnLeftGently() {
    digitalWrite(leftMotorPin1, LOW);
    digitalWrite(leftMotorPin2, LOW);
    digitalWrite(rightMotorPin1, HIGH);
    digitalWrite(rightMotorPin2, LOW);
}

void turnRightGently() {
    digitalWrite(leftMotorPin1, HIGH);
    digitalWrite(leftMotorPin2, LOW);
    digitalWrite(rightMotorPin1, LOW);
    digitalWrite(rightMotorPin2, LOW);
}

void stopMotors() {
    digitalWrite(leftMotorPin1, LOW);
    digitalWrite(leftMotorPin2, LOW);
    digitalWrite(rightMotorPin1, LOW);
    digitalWrite(rightMotorPin2, LOW);
}