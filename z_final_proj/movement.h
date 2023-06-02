

#include "open_interface.h"
#include "Timer.h"
#include "sensor.h"


#ifndef MOVEMENT_H_
#define MOVEMENT_H_



//move forwards by x cm
float move_forwards(oi_t *sensor_F, int centimeters, float a, float b);


//turn clockwise by x degrees (negative spins ccw)
float turn_clockwise(oi_t *sensor_T, int degrees);




#endif /* MOVEMENT_H_ */
