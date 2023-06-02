

#include "open_interface.h"
#include "Timer.h"


void turn_clockwise(oi_t *sensor_T, int degrees){
    float sum=0;

    if (degrees<0){
        oi_setWheels(50, -50); // turn left (right wheel forwards)
        while (sum > degrees) {
            oi_update(sensor_T);
            sum -= sensor_T->angle;
            lcd_init();
            lcd_printf("%f", sum);
        }
    }else{
        oi_setWheels(-50, 50); // turn right (left wheel forwards)
        while (sum < degrees) {
            oi_update(sensor_T);
            sum -= sensor_T->angle;
            lcd_init();
            lcd_printf("%f", sum);
        }
    }
    oi_setWheels(0, 0);
}

void bump_resL(oi_t *sensor_BL){
    oi_setWheels(-150, -150);
    double sum=0;

    while (sum<150){
        oi_update(sensor_BL);
        sum -= sensor_BL->distance;
    }

    turn_clockwise(sensor_BL, 90);
    move_forward(sensor_BL, 25);
    turn_clockwise(sensor_BL, -90);
}

void bump_resR(oi_t *sensor_BL){
    oi_setWheels(-150, -150);
    double sum=0;

    while (sum<150){
        oi_update(sensor_BL);
        sum -= sensor_BL->distance;
    }
    turn_clockwise(sensor_BL, -90);
    move_forward(sensor_BL, 25);
    turn_clockwise(sensor_BL, 90);


}


void move_forward(oi_t *sensor_F, int centimeters){
    if (centimeters>0){ // move forwards
        oi_setWheels(150, 150);

        centimeters = centimeters*10;
        double sum=0;

        while (sum < centimeters) {
            oi_update(sensor_F);
            sum += sensor_F->distance;
            if (sensor_F->bumpLeft) {
                bump_resL(sensor_F);
                oi_setWheels(150, 150);
                sum -=150;
            }
            else if (sensor_F->bumpRight) {
               bump_resR(sensor_F);
               oi_setWheels(150, 150);
               sum -=150;
            }
            else if (sensor_F->bumpLeft && sensor_F->bumpRight) {
              bump_resL(sensor_F);
              oi_setWheels(150, 150);
              sum -=150;
            }
            lcd_init();
            lcd_printf("%f", sum);
        }
    }
    else{ //move backwards
        oi_setWheels(-150, -150);

        centimeters = centimeters*10;
        double sum=0;

        while (sum > centimeters) {
            oi_update(sensor_F);
            sum += sensor_F->distance;
            lcd_init();
            lcd_printf("%f", sum);
        }
    }

    oi_setWheels(0, 0); //set wheels to 0
}

void move_noBump(oi_t *sensor_F, int centimeters){
    if (centimeters>0){ // move forwards
        oi_setWheels(150, 150);

        centimeters = centimeters*10;
        double sum=0;

        while (sum < centimeters) {
            oi_update(sensor_F);
            sum += sensor_F->distance;
            lcd_init();
            lcd_printf("%f", sum);
        }
    }
    else{ //move backwards
        oi_setWheels(-150, -150);

        centimeters = centimeters*10;
        double sum=0;

        while (sum > centimeters) {
            oi_update(sensor_F);
            sum += sensor_F->distance;
            lcd_init();
            lcd_printf("%f", sum);
        }
    }

    oi_setWheels(0, 0); //set wheels to 0
}







