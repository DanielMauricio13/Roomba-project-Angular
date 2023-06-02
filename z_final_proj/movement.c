#include "movement.h"

volatile char stopFlag;
int cliffBig = 2700; //value for white under cliff sensor

float turn_clockwise(oi_t *sensor_T, int degrees){
    float sum=0;
    stopFlag = '\0';

    if (degrees<0){
        oi_setWheels(50, -50); // turn left (right wheel forwards)
        while (sum > degrees) {
            oi_update(sensor_T);
            sum -= sensor_T->angle; //updates angle

            //cliff sensors
            if ((sensor_T->cliffLeft) | (sensor_T->cliffLeftSignal > cliffBig)){ /*if the left cliff sensor sees drop in distance or white, stop*/
                stopFlag = '1';
                oi_setWheels(0, 0); //set wheels to 0
                return sum/10;
            }
            if ((sensor_T->cliffFrontLeft) | (sensor_T->cliffFrontLeftSignal > cliffBig)){
                stopFlag = '2';
                oi_setWheels(0, 0); //set wheels to 0
                return sum/10;
            }
            if ((sensor_T->cliffFrontRight) | (sensor_T->cliffFrontRightSignal > cliffBig)){
                stopFlag = '3';
                oi_setWheels(0, 0); //set wheels to 0
                return sum/10;
            }
            if ((sensor_T->cliffRight) | (sensor_T->cliffRightSignal > cliffBig)){
                stopFlag = '4';
                oi_setWheels(0, 0); //set wheels to 0
                return sum/10;
            }
            lcd_init();
            lcd_printf("%f", sum);

        }
    }
    else{
        oi_setWheels(-50, 50); // turn right (left wheel forwards)
        while (sum < degrees) {
            oi_update(sensor_T);
            sum -= sensor_T->angle; //updates angle

            //cliff sensors
            if ((sensor_T->cliffLeft) | (sensor_T->cliffLeftSignal > cliffBig)){
                stopFlag = '1'; //used to send user what happened
                oi_setWheels(0, 0); //set wheels to 0
                return sum/10;
            }
            if ((sensor_T->cliffFrontLeft) | (sensor_T->cliffFrontLeftSignal > cliffBig)){
                stopFlag = '2';
                oi_setWheels(0, 0); //set wheels to 0
                return sum/10;
            }
            if ((sensor_T->cliffFrontRight) | (sensor_T->cliffFrontRightSignal > cliffBig)){
                stopFlag = '3';
                oi_setWheels(0, 0); //set wheels to 0
                return sum/10;
            }
            if ((sensor_T->cliffRight) | (sensor_T->cliffRightSignal > cliffBig)){
                stopFlag = '4';
                oi_setWheels(0, 0); //set wheels to 0
                return sum/10;
            }
            lcd_init();
            lcd_printf("%f", sum);
        }
    }
    oi_setWheels(0, 0);
    return sum;//return degrees turned
}


float move_forwards(oi_t *sensor_F, int centimeters, float a, float b){
    stopFlag = '\0';
    centimeters = centimeters*10;
    float sum=0;
    int count=120, dir=1, lWheelSpeed=50, rWheelSpeed=50;
    float laserDist=0, angleDeviation = 0;
	
    servo_move(135);
	timer_waitMillis(100);

    if (centimeters>0){ // move forwards

        while (sum < centimeters) {
            oi_setWheels(rWheelSpeed, lWheelSpeed);
            oi_update(sensor_F);
            sum += sensor_F->distance;
            angleDeviation -= sensor_F->angle;

            //bump sensors
            if (sensor_F->bumpLeft && sensor_F->bumpRight) { //if the left and right bump is hit stop
                stopFlag = 'B'; //to send to user what happened
                oi_setWheels(0, 0); //set wheels to 0
                return sum/10;
            }
            if (sensor_F->bumpLeft) {
                stopFlag = 'L';
                oi_setWheels(0, 0); //set wheels to 0
                return sum/10;
            }
            if (sensor_F->bumpRight) {
                stopFlag = 'R';
                oi_setWheels(0, 0); //set wheels to 0
                return sum/10;
            }

            //cliff sensors
            if ((sensor_F->cliffLeft) | (sensor_F->cliffLeftSignal > cliffBig)){ /*if the left cliff sensor sees drop in distance or white, stop*/
                stopFlag = '1';
                oi_setWheels(0, 0); //set wheels to 0
                return sum/10;
            }
            if ((sensor_F->cliffFrontLeft) | (sensor_F->cliffFrontLeftSignal > cliffBig)){
                stopFlag = '2';
                oi_setWheels(0, 0); //set wheels to 0
                return sum/10;
            }
            if ((sensor_F->cliffFrontRight) | (sensor_F->cliffFrontRightSignal > cliffBig)){
                stopFlag = '3';
                oi_setWheels(0, 0); //set wheels to 0
                return sum/10;
            }
            if ((sensor_F->cliffRight) | (sensor_F->cliffRightSignal > cliffBig)){
                stopFlag = '4';
                oi_setWheels(0, 0); //set wheels to 0
                return sum/10;
            }

            //angle correction
            //if turning left, sensor angle goes up (left wheel has to spin faster)
            if (angleDeviation>0){
                lWheelSpeed -= 1;
            }
            else if (angleDeviation<0){
                lWheelSpeed += 1;
            }
            //lcd_printf("dev: %f\nright: %d\nleft: %d", angleDeviation, rWheelSpeed, lWheelSpeed);
            angleDeviation = 0;


            //sweep sensor
			if (dir>0){ //moving right (angle decreases)
                servo_move(count);
                count-=8;
            }
            else if (dir<0){
                servo_move(count);
                count+=8;
            }
			
			if (count <= 45){
				dir = -1;
			}
			else if (count >= 135){
				dir = 1;
			}


            laserDist = 0; //laserADC read
            laserDist += laser_read();
            laserDist += laser_read();
            laserDist += laser_read();
            laserDist /= 3;

            if (laserDist > 1500){ //less than ~30 cm (arbitrarily chosen)
                //stopFlag = 'O'; //O for Object (seen)
                oi_setWheels(0, 0); //set wheels to 0
				return sum/10;
            }
        }
    }
    else{ //move backwards
        lWheelSpeed=-50, rWheelSpeed=-50;

        while (sum > centimeters) {
            oi_setWheels(rWheelSpeed, lWheelSpeed);
            oi_update(sensor_F);
            sum += sensor_F->distance;
            angleDeviation -= sensor_F->angle;

            if (angleDeviation>0){
                lWheelSpeed -= 1;
            }
            else if (angleDeviation<0){
                lWheelSpeed += 1;
            }
            angleDeviation = 0;
        }
    }

    oi_setWheels(0, 0); //set wheels to 0
    return sum/10; //sum in cm
}







