


#include "sensor.h"
#include "Timer.h"
#include "button.h"
#include "lcd.h"
#include "uart.h"
#include "movement.h"
#include "open_interface.h"
#include <inc/tm4c123gh6pm.h>
#include "string.h"

extern volatile int uart_event;
extern volatile char uart_data;
extern volatile char stopFlag;

//set this up in a different file (lab 8 main)
//BOT 8:
const float a_las=1862896.375, b_las=-1.523016;

//BOT 10: //const float a_las=7321145, b_las=-1.802885;


const float degToRad = 3.141592653/180;
const int minObjDist = 55; //minimum distance detected by ping sensor for something to be considered an "object"


float dist;
char puttyIn[21] = "";



struct tallObj { //contains data for all tall objects
    int startAng;
    int stopAng;
    float objDist;
    float width;
    int actualAng;
};
struct tallObj tObject[5];

       //measures 180 degrees, stores object amount, location, and size
int measure_data(){
    int i, j, laserADC, midAng;
    float laserDist;


    int isObject = 0;
    int numObjects = 0;

    tObject[0].stopAng=0;

    for (j=0; j<=180; j+=2){

        servo_move(j);

        laserADC = 0;
        for (i=0; i<3; i++){
            laserADC += laser_read();
        }
        laserADC /= 3;

        //print angle
        //strcpy(puttyIn,"");
        //sprintf(puttyIn, "%d\t", j);
        //uart_sendStr(puttyIn);


        //get/print ir distance
        laserDist = a_las * pow(laserADC, b_las);
        //sprintf(puttyIn, "%f\r\n", laserDist);
        //uart_sendStr(puttyIn);


        if ( (laserDist<minObjDist) && (isObject==0) ){ //detects object
            isObject=1;
            tObject[numObjects].startAng = j;
        }
        if ( (laserDist>minObjDist) && (isObject== 1) ){ //detects lack of object
            tObject[numObjects].stopAng = j-2;
            isObject=0;
            if ((tObject[numObjects].stopAng - tObject[numObjects].startAng) > 3){
                        //verifies object exists and is not a sensor scuffing data
                numObjects++;
            }

        }
    }//finish measuring


    uart_sendStr("\r\n\nObject\tAngle\tDistance\tWidth\r\n"); //header of object info


    //prints objects
    for (i=0; i<numObjects;i++){ //numObjects begins reading useful data at i=1
        midAng = ((tObject[i].stopAng + tObject[i].startAng)/2); //middle angle of object
        servo_move(midAng);
        timer_waitMillis(800);
        tObject[i].objDist = ping_read();
        timer_waitMillis(100);


        tObject[i].width = (tObject[i].objDist+4) * 2 * tan( (tObject[i].stopAng - tObject[i].startAng)*degToRad/2 ); //measures width of object (cm)


        strcpy(puttyIn,"");
        sprintf(puttyIn, "%d\t", i);
        uart_sendStr(puttyIn); //sends object #

        sprintf(puttyIn, "%d\t", midAng);
        uart_sendStr(puttyIn); //sends middle angle

        sprintf(puttyIn, "%f\t", tObject[i].objDist);
        uart_sendStr(puttyIn); //sends distance away

        sprintf(puttyIn, "%f\r\n", tObject[i].width);
        uart_sendStr(puttyIn); //sends linear width

        tObject[i].actualAng = ((atan( ((tObject[i].objDist+5.0)*cos(midAng*degToRad)) /
                                      (11.0+(tObject[i].objDist+5.0)*sin(midAng*degToRad)) )/degToRad) + 0.5);
                //center of bot (COB) / center of sensor (COS)
                //5 is cm distance between COS and actual measuring point
                //11 is cm distance between COB/COS
    }
    uart_sendStr("Done\r\n\n");
	return numObjects;
}








void main(){
    //SYSCTL_RCGCUART_R |= 0b10;      // enable clock UART1 (page 344)
    //SYSCTL_RCGCGPIO_R |= 0b10;      // enable clock GPIOB (page 340)
    //GPIO_PORTB_DEN_R |= 0b10000000;        // enables pb7
    //GPIO_PORTB_DIR_R |= 0b10000000;        // sets pb7 as output
    //GPIO_PORTB_DATA_R |= 0b10000000;        // sets pb7 high (reset)
    init_sensors();
    timer_init();
    lcd_init();
    button_init();

    uart_init();
    uart_interrupt_init();
    uart_event=0;


    oi_t *sensor_data = oi_alloc();
    oi_init(sensor_data);



    int numChars=0;
	int amountScanned;
	float amountMoved;
    char whatDo;
    int amountDo, amountDo2;

	amountMoved = 0;
	uart_sendStr("Input: ");
	
	char notes[] = {74, 67, '\0'};
	char duration[] = {64, 80, '\0'};

	//frees memory

	free(notes); 
	free(duration); 
	
	oi_loadSong(0, 2, notes, duration);

    char * amounts; //creates pointer "amounts" that stores data based on comma delimiter
    char * str = malloc(21 * sizeof(char));

	//calibration for cliff analog value (in movement.c) and servo tmax/min (in sensor.c)
	/*amountDo = sensor_data->cliffLeftSignal;
	amountDo = sensor_data->cliffFrontLeftSignal;
	amountDo = sensor_data->cliffFrontRightSignal;
	amountDo = sensor_data->cliffRightSignal;

	servo_calibrate();*/
	
    while(1){

        if (uart_event){ //if putty detects a keypress
            uart_event = 0;

            puttyIn[numChars] = uart_data;
            numChars++;

            uart_sendChar(uart_data);

            if (uart_data == '\r'){ //once send character is detected, begin operation (send character is newline)

				//amountDo = atoi(puttyIn); //converts numbers in the string to an int until something that isn't a number is reached
				whatDo = puttyIn[numChars-2]; //converts last user input before enter key into a command

				//creates pointer str that will have relevant puttyIn data copied into it
				strcpy(str, puttyIn);

				//removes whitespace in front of str (shouldn't be an issue because str is directly copied from puttyIn
				//while(isspace((unsigned char)str)) str++;


				amounts = strtok(str, ",");         //fills "amounts" pointer with any data from "str" (puttyIn) until comma
				amountDo = atoi(amounts);           //converts "amounts" string into integer
				//testing
				/*sprintf(puttyIn, "%d %d", amountDo, amountDo2);
                uart_sendStr(puttyIn);*/


				amounts = strtok(NULL, ",");        //In the next call to strtok, the first parameter needs to be NULL so that strtok starts splitting the string from the next token's starting position it remembers.
				amountDo2 =  atoi(amounts);     //converts new "amounts" string into integer
				//testing
				/*sprintf(puttyIn, "%d %d", amountDo, amountDo2);
                uart_sendStr(puttyIn);*/

				//frees str and amounts pointers

				uart_sendChar('\r');
				uart_sendChar('\n');

                switch (whatDo){
                case ('q'):
                        //testing
                        /*sprintf(puttyIn, "%d %d", amountDo, amountDo2);
                        uart_sendStr(puttyIn);*/

                        uart_sendStr("START turn ");

                        amountMoved = turn_clockwise(sensor_data, amountDo);

                        sprintf(puttyIn, "%f ", amountMoved);
                        uart_sendStr(puttyIn);
                        uart_sendStr("END\n\r");

                        uart_sendStr("START move ");

                        amountMoved = move_forwards(sensor_data, amountDo2, a_las, b_las);

                        sprintf(puttyIn, "%f ", amountMoved);
                        uart_sendStr(puttyIn);
                        uart_sendStr("END\n\r");
                        break;

                /*case ('w'): //w is move forwards (cm)
                        uart_sendStr("START move ");

                        amountMoved = move_forwards(sensor_data, amountDo, a_las, b_las);

                        sprintf(puttyIn, "%f ", amountMoved);
                        uart_sendStr(puttyIn);
                        uart_sendStr("END\n\r");
                        break;

                case ('s'): //s is move backwards (cm)
                        uart_sendStr("Moving back (deprecated)\n\r");
                        amountDo *= -1;
                        amountMoved = move_forwards(sensor_data, amountDo, a_las, b_las);
                        uart_sendStr("Finished, but you shouldn't see this\r\n\n");
                        break;

                case ('a'): //a is rotate left (degrees)
                        uart_sendStr("START turn ");
                        amountDo *= -1;

                        amountMoved = turn_clockwise(sensor_data, amountDo);

                        sprintf(puttyIn, "%f ", amountMoved);
                        uart_sendStr(puttyIn);
                        uart_sendStr("END\n\r");
                        break;

                case ('d'): //d is rotate right (degrees)
                        uart_sendStr("START turn ");

                        amountMoved = turn_clockwise(sensor_data, amountDo);

                        sprintf(puttyIn, "%f ", amountMoved);
                        uart_sendStr(puttyIn);
                        uart_sendStr("END\n\r");
                        break;

                case ('c'): //calibrate
                        uart_sendStr("Calibrating\r\n\n");
                        servo_calibrate();
                        break;*/

                case ('f'): //frees all data and then reinitializes robot
                    oi_free(sensor_data);
                    free(str);
                    free(amounts);
                    str = NULL;
                    amounts = NULL;
                    break;

                default:
                    uart_sendStr("Invalid Input\r\n\n");
                }//end of switch
				
                if (stopFlag == 'L'){
                    uart_sendStr("START bump l END\r\n"); //sends user why bot stopped
                }
                if (stopFlag == 'R'){
                    uart_sendStr("START bump r END\r\n");
                }
                if (stopFlag == 'B'){
                    uart_sendStr("START bump b END\r\n");
                }
                if (stopFlag == '1'){
                    uart_sendStr("START cliff ll END\r\n");
                }
                if (stopFlag == '2'){
                    uart_sendStr("START cliff ml END\r\n");
                }
                if (stopFlag == '3'){
                    uart_sendStr("START cliff mr END\r\n");
                }
                if (stopFlag == '4'){
                    uart_sendStr("START cliff rr END\r\n");
                }
                if (stopFlag != '\0'){  //continues once stop flag cleared or not there
                    uart_sendStr("START move ");
					oi_play_song(0);
                    amountMoved = move_forwards(sensor_data, -15, a_las, b_las);
                    sprintf(puttyIn, "%f ", amountMoved);
                    uart_sendStr(puttyIn);
                    uart_sendStr("END\n\r");
                }


				if (whatDo == 'q'){ //measure after moving
					//scan 180 degrees and send to putty
					//uart_sendStr("Ang\tDist (ping)\tDist (laser)\r\n"); //header of measurement

					servo_move(0);
					timer_waitMillis(1000);

					amountScanned = measure_data();

					for (numChars = 0; numChars<amountScanned; numChars++){ //funny memory conservation
					    uart_sendStr("START obj ");
						//sprintf(puttyIn, " %d", numChars);
						//uart_sendStr(puttyIn);
						sprintf(puttyIn, "%d, ", tObject[numChars].actualAng);
						uart_sendStr(puttyIn);
						sprintf(puttyIn, "%f, ", tObject[numChars].objDist);
						uart_sendStr(puttyIn);
						sprintf(puttyIn, "%f ", tObject[numChars].width);
						uart_sendStr(puttyIn);
						uart_sendStr("END\n\r");
					}
				}
				numChars=0; //reset
				uart_sendStr("START READY END\r\n\nInput: ");
            }
            //GPIO_PORTB_DEN_R |= 0b10000000;        // enables pb7
            //GPIO_PORTB_DIR_R |= 0b10000000;        // sets pb7 as output
            //GPIO_PORTB_DATA_R &= 0b01111111;        // sets pb7 low (reset)
        }





    }



}




