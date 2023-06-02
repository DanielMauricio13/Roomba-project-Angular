


//#include "uart.h"
#include "Timer.h"
#include "button.h"
#include "lcd.h"
#include <inc/tm4c123gh6pm.h>




void laser_init(void);

int laser_read(void);



void ping_init(void);

float ping_read(void);




void servo_init(void);

void servo_move(int ang);

void servo_calibrate(void);


void init_sensors(void);
