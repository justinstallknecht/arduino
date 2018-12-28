//Questions:
//what is the keyword for flash memory?
// A: PROGMEM - https://www.arduino.cc/reference/en/language/variables/utilities/progmem/
// ----- this is not going to be simple, but short of separately running a raspberry pi and offloading
//       all large scale mapping to it (which would allow collabratively behaviour between multiple
//       bots), i don't think there is much in the way of alternatives
// ----- possible alternative: a dedicated higher end arduino (with 8KB of SRAM) only doing the mapping
//       functions and communicating over the single wire bus
//
//how to once you have avoided an obstacle come back to the original path? (needed in mapping)
// A: Maybe store the net time left and right but one would also have to use
//    the distance traveled to get back
//    so, I think that we have to use vectors and think about distance rather than time
// ----- yes, I think that is essentially correct, store a "current intended direction is blah blah", and
//       perhaps a "current adjusted direction to avoid obstacle at so and so is goo goo"
//
//What is the syntax for a byte array?
// A: byte mapArray[32][32]; - https://forum.arduino.cc/index.php?topic=220385.0
// ----- there does seem to be some complexities about this sometimes and there is no range checking,
//       perhaps we should look for helpful libraries?
//


#include "MeMCore.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

//objects used
MeUltrasonicSensor ultraSensor(3);
MeDCMotor motor_9(9);
MeDCMotor motor_10(10);

//motor fields
int mode;
int time;
int speed;

//timing fields
int last_checked = 0;
int delayCheck = 100;

//navigation fields
double current_x, current_y, intended_x, intended_y;
double current_angle;  //in radians
int obstacle = 0;

//from MeMCore example set
void move(int direction, int speed) {
      int leftSpeed = 0;
      int rightSpeed = 0;
      if(direction == 1){
        	leftSpeed = speed;
        	rightSpeed = speed;
      }else if(direction == 2){
        	leftSpeed = -speed;
        	rightSpeed = -speed;
      }else if(direction == 3){
        	leftSpeed = -speed;
        	rightSpeed = speed;
      }else if(direction == 4){
        	leftSpeed = speed;
        	rightSpeed = -speed;
      }
      // Q: do you understand the below two lines yet? if not, I will rewrite them in clearer syntax so you can follow
      motor_9.run((9)==M1?-(leftSpeed):(leftSpeed));
      motor_10.run((10)==M1?-(rightSpeed):(rightSpeed));
}

int now(){
    return millis(); //the number of milliseconds since program start
}

void stop(){
    move(1, 0);
}

void forward(){
    mode = 1;
    move(mode,speed);
    time = now();
}

void backward(){
    mode = 2;
    move(mode,speed);
    time = now();
}

void left(){
    mode = 3;
    move(mode,speed);
    time = now();
}

void right(){
    mode = 4;
    move(mode,speed);
    time = now();
}

double distance(){
    double distance = sqrt(pow(ultraSensor.distanceCm(),2)-9);  //corects for close distance error
                                                                // ----- we still need to test this, and potentially
                                                                //       filter out negatives and spurious values
    return distance;
}

void setup(){
    Serial.print("START");
    speed = 100;
    last_checked = 0;
    Serial.begin(9600);
}


//left to right distance scanning
//Goal: store values in a array of distances to find obstacles
int[] lookAround(){
    int scan [5];
    move(3,40);
    delay(50);
    scan[0] = distance();
    for(int i = 0; i<4; i++){
        move(4,40);
        scan[i+1] = distance();
    }
    move(3,40);
    delay(50);
    return scan;
}

void loop(){
    if ((now() - last_checked) > delayCheck){
        //if current(x,y) == intended(x,y)
        //update intended(x/y) by some algoriethm
        last_checked = now();

        //scan
        int scan [5] = lookAround();

        // get minimum distance from scan
        int minDist = 400; //it is set equal to maximum distance
        for(int i = 0; i<5; i++){
            if(scan[i]<minDist){
                minDist = scan[i];
            }
        }

        if (minDist<10){  // detected obstacle

            //printing to serial
            Serial.print("OBSTACLE - distance: ");
            Serial.print(dist);
            Serial.print(", count: ");
            Serial.println(count);

            //manuver around obstacle with preference for going twords intended(x,y)
            obstacle = 1;
            //decide which directing to turn based on obstacle thats further away in that direction
            if((scan[0]+scan[1])<(scan[3]+scan[4])){ //left?
                left();
                current_angle -= 0.3; //subtracts n radians
            } else { //right?
                right();
                current_angle += 0.3; //adds n radians
            }

        } else { //no obstacle - eventually navagate twords intended (x,y)
            obstacle = 0;

            count = 0;
            forward();

            //update current(x,y)
            //4 here is completly abatrary distance
            current_x += 4 * cos(current_angle);
            current_y += 4 * sin(current_angle);
        }
    }   //possibly "else" with delay such that it doesn't use too much processing
        // or do math with the array map
        // ----- this is an excellent concept, to go to the extreme end we could setup
        //       a queue of processing tasks with time estimates for completion to
        //       give us reasonably efficient use of the downtime whilst also
        //       maintaining responsiveness in the rest of the loop, but we should
        //       probably not jump straight to that level of insanity
}

//picks a direction based off of "ran" (random) variable
// ----- there is something a little odd going on here with the way ran is used inside and outside
//       the proc, we should be able to clean this up some
void pickADirection(){
    if (ran == 1){
        Serial.println("PICKED LEFT");
        left();
    } else { // 2
        Serial.println("PICKED RIGHT");
        right();
    }
}
