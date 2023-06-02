


#include "timer.h"
#include "lcd.h"
#include "uart.h"
#include "math.h"
#include "stdlib.h"
#include "sensor.h"
#include "string.h"
#include "movement.h"
#include <cyBot_Scan.h>
#include "inc/tm4c123gh6pm.h"


extern volatile int uart_event;
extern volatile char uart_data;


const float degToRad = 3.141592653/180;
int minAngWhere, minAngWid;
float dist;
char puttyIn[51] = "";


       //input r/l angles, cybot data variable and pointer, and power data from laser calibration
void measure_data(int arcLen,  cyBOT_Scan_t scanData, cyBOT_Scan_t* scanDataPtr, float a_las, float b_las){
    int i, j, laserADC;
    float laser_dist_num;
    scanDataPtr = &scanData;

    const int minObjDist = 60; //minimum distance detected by ping sensor for something to be considered an "object"
    int startAng[8];
    int stopAng[8];
    float objDist[8];
    int isObject = 0;
    int numObjects = 0;
    int leftScan = (90+arcLen/2);
    int rightScan = (90-arcLen/2);

    minAngWhere = 90;


    int midAng;
    float linWidth;


    stopAng[0]=0;

    for (j=rightScan; j<=leftScan; j+=2){
        cyBOT_Scan(j, scanDataPtr);
        if (uart_event & (uart_data == 't')){
            break;
        }
        laserADC = 0;
        for (i=0; i<3; i++){
            laserADC += laser_read();
        }
        laserADC /= 3;

        //print angle
        strcpy(puttyIn,"");
        sprintf(puttyIn, "%d\t", j);
        uart_sendStr(puttyIn);

        //print distance (ping)
        sprintf(puttyIn, "%f\t", scanData.sound_dist);
        uart_sendStr(puttyIn);


        //get/print ir distance
        laser_dist_num = a_las * pow(laserADC, b_las);
        sprintf(puttyIn, "%f\r\n", laser_dist_num);
        uart_sendStr(puttyIn);


        if ( (laser_dist_num<minObjDist) && (isObject==0) ){ //detects object
            isObject=1;
            startAng[numObjects+1]=j;
            objDist[numObjects+1]=scanData.sound_dist;
        }
        if ( (laser_dist_num>minObjDist) && (isObject== 1) ){ //detects lack of object
            stopAng[numObjects+1]=j-2;
            isObject=0;
            if ((stopAng[numObjects+1]-startAng[numObjects+1]) > 3){ //verifies object exists and is not a sensor scuffing data
                numObjects++;
            }

        }
    }//finish measuring


    uart_sendStr("\r\n\nObject\tAngle\tDistance\tWidth\r\n"); //header of object info

    if (numObjects>=1){
        minAngWhere = (stopAng[1] + startAng[1])/2;
        minAngWid = stopAng[1] - startAng[1];
        dist = objDist[1];
    }


    //prints objects
    for (i=1; i<=numObjects;i++){ //numObjects begins reading useful data at i=1
        midAng = ((stopAng[i] + startAng[i])/2); //middle angle of object
        timer_waitMillis(100);
        cyBOT_Scan(midAng, scanDataPtr);
        objDist[i] = scanData.sound_dist;
        timer_waitMillis(500);


        linWidth = objDist[i] * 2 * sin( (stopAng[i] - startAng[i])*degToRad/2 ); //measures width of object (cm)


        strcpy(puttyIn,"");
        sprintf(puttyIn, "%d\t", i);
        uart_sendStr(puttyIn); //sends object #

        sprintf(puttyIn, "%d\t", midAng);
        uart_sendStr(puttyIn); //sends middle angle

        sprintf(puttyIn, "%f\t", objDist[i]);
        uart_sendStr(puttyIn); //sends distance away

        sprintf(puttyIn, "%f\r\n", linWidth);
        uart_sendStr(puttyIn); //sends linear width

        if ((stopAng[i]-startAng[i]) < minAngWid){  //if the current object width is smaller than the previous
            minAngWhere = midAng;                   //sets point to mid angle of (current) smallest width object
            minAngWid = stopAng[i]-startAng[i];
            dist = objDist[i];
        }
    }
    uart_sendStr("Done\r\n\n");
}







void main(){
    timer_init();
    lcd_init();
    uart_init();
    uart_interrupt_init();
    uart_event=0;
    laser_init();

    cyBOT_init_Scan(0b0011);
    cyBOT_Scan_t scanData;
    cyBOT_Scan_t* scanDataPtr = &scanData;
    oi_t *sensor_data = oi_alloc();
    oi_init(sensor_data);
    right_calibration_value = 259000;
    left_calibration_value = 1246000;

    float a=1653853.875000, b=-1.581779;

    int numChars=0;
    char whatDo;
    int amountDo;

    int count, isObj, offset;
    float temp;




    while(1){

        if (uart_event){ //if putty detects a keypress
            uart_event = 0;

            puttyIn[numChars] = uart_data;
            numChars++;

            uart_sendChar(uart_data);

            if (uart_data == '\r'){ //if enter key pressed      enter distance/angle and wasd char

                amountDo = atoi(puttyIn); //converts numbers in the string to an int until something that isn't a number is reached
                whatDo = puttyIn[numChars-2]; //converts last user input before enter key into a command

                numChars=0; //reset
                uart_sendChar('\r');
                uart_sendChar('\n');
                //uart_sendChar(whatDo);

                switch (whatDo){
                case ('w'): //w is move forwards (cm)
                        sprintf(puttyIn, "Moving forwards for %d cm\r\n", amountDo);
                        uart_sendStr(puttyIn);
                        move_noBump(sensor_data, amountDo);
                        uart_sendStr("Done\r\n\n");
                        break;

                case ('s'): //s is move backwards (cm)
                        sprintf(puttyIn, "Moving backwards for %d cm\r\n", amountDo);
                        uart_sendStr(puttyIn);
                        amountDo *= -1;
                        move_noBump(sensor_data, amountDo);
                        uart_sendStr("Done\r\n\n");
                        break;

                case ('a'): //a is rotate left (degrees)
                        sprintf(puttyIn, "Turning left for %d degrees\r\n", amountDo);
                        uart_sendStr(puttyIn);
                        amountDo *= -1;
                        turn_clockwise(sensor_data, amountDo);
                        uart_sendStr("Done\r\n\n");
                        break;

                case ('d'): //d is rotate right (degrees)
                        sprintf(puttyIn, "Turning right for %d degrees\r\n", amountDo);
                        uart_sendStr(puttyIn);
                        turn_clockwise(sensor_data, amountDo);
                        uart_sendStr("Done\r\n\n");
                        break;

                case ('j'): //laser calibaration
                        uart_sendStr("Get value from lab 8\r\n\n");
                        break;

                case ('t'): //measure and schmove (automatic) (input is arc length in degrees)
                        uart_sendStr("Auto Mode\r\n");
                        uart_sendStr("Ang\tDist (ping)\tDist (laser)\r\n"); //header of measurement

                        cyBOT_Scan(0, scanDataPtr);
                        timer_waitMillis(100);

                        measure_data(180, scanData, scanDataPtr, a, b);

						//amount should turn clockwise to correct
                        //(taking into account discrepancy between center of bot (COB) / center of sensor (COS)
                        dist += 5; //adds 5 cm from cos to actual scan start
                        temp = atan( (dist*cos(minAngWhere*degToRad)) / (11+dist*sin(minAngWhere*degToRad)) )/degToRad;
                                                                //11 is cm distance between COB/COS
                        sprintf(puttyIn, "\n\rcorrect turn angle: %f\r\n", temp);
                        uart_sendStr(puttyIn);

						turn_clockwise(sensor_data, (temp+0.5)); //turns towards object
						measure_data(45, scanData, scanDataPtr, a, b);
						offset = 90-minAngWhere; //generates new offset needing to be corrected
						                         //doesn't need formula, only care about positive/negative
						
						cyBOT_Scan(90, scanDataPtr);
						count=0;

                        /*while (offset != 0){
							dist=0;
							dist += a * pow(laser_read(), b); //assigns a variable to laser read
							dist += a * pow(laser_read(), b);
							dist += a * pow(laser_read(), b);
							dist /= 3; //average distance detected
							
							isObj=0;


							if (dist < 45){ //if something in front of the laser sensor
								count ++; //count how many rotations it is front of the laser for
								isObj = 1;
							    //offset = 0; //halts while loop
							}

							if ( (isObj==0) * (count>0) ){ //once object no longer in view
								offset *= -1; //inverts offset so robot turns in other direction
								while (count > 0){
									if (offset > 0){ //needs to turn CW
										turn_clockwise(sensor_data, 1);
									}
									if(offset < 0){ //needs to turn CCW
										turn_clockwise(sensor_data, -1);
									}
									count -= 2; //decrements by 2 so that the robot ends up pointing at the middle of the object
								}
								offset = 0; //halts while loop
							}


							if (offset > 0){ //needs to turn CW
								turn_clockwise(sensor_data, 1);
							}
							if(offset < 0){ //needs to turn CCW
								turn_clockwise(sensor_data, -1);
							}
						}*/


                        cyBOT_Scan(90, scanDataPtr);
                        while (scanData.sound_dist > 10){

                            if (uart_event & (uart_data == 't')){
                                break;
                            }
                            dist = scanData.sound_dist*0.5;
                            move_forward(sensor_data, (dist));
                            cyBOT_Scan(90, scanDataPtr);
                        }

                        break; //end of auto case

                case ('m'): //measure (input is arc length in degrees)
                    sprintf(puttyIn, "Arc Length: %d\r\n", amountDo);
                    uart_sendStr(puttyIn);

                    uart_sendStr("Ang\tDist (ping)\tDist (laser)\r\n"); //header of measurement

                    cyBOT_Scan((90-amountDo/2), scanDataPtr);
                    timer_waitMillis(100);

                    measure_data(amountDo, scanData, scanDataPtr, a, b);


                    //amount should turn clockwise to correct
                    //(taking into account discrepancy between center of bot (COB) / center of sensor (COS)
                    temp = atan( (dist*cos(minAngWhere*degToRad)) / (11+dist*sin(minAngWhere*degToRad)) )/degToRad;
                                                                                    //11 is cm distance between COB/COS
                    sprintf(puttyIn, "\n\rcorrect turn angle: %f\r\n", temp);
                    uart_sendStr(puttyIn);



                    break; //end of measure case

                case ('c'): //calibrate
                        uart_sendStr("Calibrating\r\n\n");
                        cyBOT_SERVO_cal();
                        break;

                case ('f'):
                    oi_free(sensor_data);
                    break;

                default:
                    uart_sendStr("Invalid Input\r\n\n");
                }

            }
        }





    }



}





