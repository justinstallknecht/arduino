#include <Servo.h>

Servo servoRight;
Servo servoLeft;
Servo servoPing;

const int pingPin = 7;

float scan[18], dist, tmp;

int direct;

void setup() {
  // initialize serial communication:
  Serial.begin(9600);

  servoRight.attach(12);
  servoLeft.attach(13);
  servoPing.attach(11);

  for(int i=0; i<18; i++){
      scan[i] = float(122);
  }

}

void loop() {
    for(int i=0; i<180; i+=10){
        senseMove(i);
    }
    for(int i=180; i>0; i-=10){
        senseMove(i);
    }
}

void senseMove(int angle){
    servoPing.write(angle);
    scan[(angle/10)+1] = distance();
    dist = minDist();
    Serial.print(dist);
    Serial.println();
    if (dist <= 4) {
        slowDown(5);
        backward(1000);
        if (direct<=90){
            turnLeft(500+(direct*150));
        }
        else{
            turnRight(500+(direct*150));
        }
        for(int i=0; i<180; i+=10){
            servoPing.write(i);
            scan[(i/10)+1] = distance();
        }
    }
    else if (dist <= 12) {
        forward(50, 200);
    }
    else{
        forward(50, 200);
    }
}

float distance(){
    long duration;
    float inches;
    delay(20);

    // The PING))) is triggered by a HIGH pulse of 2 or more microseconds.
    // Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
    pinMode(pingPin, OUTPUT);
    digitalWrite(pingPin, LOW);
    delayMicroseconds(2);
    digitalWrite(pingPin, HIGH);
    delayMicroseconds(5);
    digitalWrite(pingPin, LOW);

    // The same pin is used to read the signal from the PING))): a HIGH
    // pulse whose duration is the time (in microseconds) from the sending
    // of the ping to the reception of its echo off of an object.
    pinMode(pingPin, INPUT);
    duration = pulseIn(pingPin, HIGH);

    // convert the time into a distance
    inches = microsecondsToInches(duration);
    return inches;
}

float microsecondsToInches(long microseconds) {
  //turning microsecounds to inches.
  //74 n speed of sound
  return (float(microseconds) / 74) / 2;
}

void forward(int time, int speed){
    servoLeft.writeMicroseconds(1500+speed);
    servoRight.writeMicroseconds(1500-speed);
    delay(time);
}

void turnLeft(int time){
    servoLeft.writeMicroseconds(1300);
    servoRight.writeMicroseconds(1300);
    delay(time);
}

void turnRight(int time){
    servoLeft.writeMicroseconds(1700);
    servoRight.writeMicroseconds(1700);
    delay(time);
}

void backward(int time){
    for(int i=0; i<200; i+=10){
        servoLeft.writeMicroseconds(1500-i);
        servoRight.writeMicroseconds(1500+i);
        delay(100);
    }
}

void slowDown(int howmuch){
    for(int i=200; i>0; i-=10){
        servoLeft.writeMicroseconds(1500+i);
        servoRight.writeMicroseconds(1500-i);
        delay(50);
    }
}

float minDist(){
    float min = 122;
    for(int i = 0; i < 18; i++){
        if (scan[i] <= min){
            min = scan[i];
            direct = i*10;
        }
    }
    return min;
}
