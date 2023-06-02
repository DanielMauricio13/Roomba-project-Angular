
/*
#include "Timer.h"
#include "sensor.h"
#include "lcd.h"
#include "uart.h"


extern volatile int overflow_event;


void main(void)
{
    ping_init();
    lcd_init();
    uart_init();
    uart_interrupt_init();
    //uart_event=0;

    float laserData;
    int overflows = 0;
    overflow_event = 0;


	while (1){
	    laserData = ping_read();

	    if (overflow_event == 1){
	        overflows++;
	        //overflow_event = 0;

	    }
        lcd_printf("ping: %f\nOverflows: %d", laserData, overflows);
        while(overflow_event == 1){
            timer_waitMillis(25000);
            overflow_event=0;
        }
	    timer_waitMillis(20);


	}
}
//*/


/**/
#include "sensor.h"

#include "timer.h"
#include "lcd.h"
#include "uart.h"
#include "math.h"
#include "movement.h"


extern volatile int uart_event;
extern volatile char uart_data;

 void main(){
    timer_init();
    lcd_init();
    uart_init();
    uart_interrupt_init();
    laser_init();
    ping_init();
    uart_event=0;



    int dataPoints=20;
    float x[22], y[22], sumX=0, sumX2=0, sumY=0, sumXY=0, A;
    char puttyIn[60] = "";

    //wheels
    oi_t *sensor_data = oi_alloc();
    oi_init(sensor_data);

    //sensor
    //cyBOT_init_Scan(0b0111);
    //cyBOT_Scan_t scanData;
    //cyBOT_Scan_t* scanDataPtr = &scanData;
    //right_calibration_value = 259000;
    //left_calibration_value = 1246000;


    float a=140841.515625, b=-1.167015;
    int i, j;
    int laserData;
    float dist;




    while(1){

        if (uart_event){ //if putty detects a keypress
            uart_event = 0;

            switch (uart_data){

                case ('m'): //print laser raw value and laser actual value
                    laserData = laser_read();
                    dist = a * pow(laserData, b);
                    //cyBOT_Scan(90, scanDataPtr);
                    sprintf(puttyIn, "raw: %d\tactual: %f\tlaserCalc: %f\r\n", laserData, ping_read(), dist);
                    uart_sendStr(puttyIn);
                    lcd_printf("laser raw: %d\nactual: %f\nLasCalc: %f", laserData, ping_read(), dist);
                    break;

                case ('j'): //laser calibaration
                    sprintf(puttyIn, "Calibrating Laser Value\r\n");
                    uart_sendStr(puttyIn);

                    for(i=1;i<=dataPoints;i++){//measure points, wait, move, wait
                        x[i] = 0;
                        y[i] = 0;
                        for(j=0; j<3; j++){
                            //cyBOT_Scan(90, scanDataPtr);

                            x[i] += laser_read();
                            y[i] += ping_read();
                        }
                        x[i] /= 3;
                        y[i] /= 3;

                        sprintf(puttyIn, "%f\n\r", x[i]);
                        uart_sendStr(puttyIn);

                        timer_waitMillis(100);
                        move_noBump(sensor_data, -1);
                        timer_waitMillis(300);
                    }

                    // Calculating Required Sum
                    sumX=0, sumX2=0, sumY=0, sumXY=0;
                    for(i=1;i<=dataPoints;i++){
                        sumX += log(x[i]);
                        sumX2 += log(x[i])*log(x[i]);
                        sumY += log(y[i]);
                        sumXY += log(x[i])*log(y[i]);
                    }

                    // Calculating A and b
                    b = (dataPoints*sumXY-sumX*sumY)/(dataPoints*sumX2-sumX*sumX);
                    A = (sumY - b*sumX)/dataPoints;

                    // Transformation of A to a
                    a = exp(A);

                    //equation: a*x^b
                    sprintf(puttyIn, "equation: %f*x^%f\r\n", a, b);
                    uart_sendStr(puttyIn);
                    uart_sendStr("Done\r\n\n");
                    break;

                case ('f'):
                    oi_free(sensor_data);
                    break;

                case ('c'): //calibrate
                    uart_sendStr("Calibrating\r\n\n");
                    //cyBOT_SERVO_cal();
                    break;

                default:
                    uart_sendStr("Invalid Input\r\n\n");

            }
        }
    }



}
//*/
