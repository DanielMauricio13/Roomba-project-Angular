

#include "open_interface.h"
#include "Timer.h"


#ifndef MOVEMENT_H_
#define MOVEMENT_H_



//move forwards by x cm
void move_forward(oi_t *sensor_F, int centimeters);

//move forwards by x cm (does not check bump sensors)
void move_noBump(oi_t *sensor_F, int centimeters);

//turn clockwise by x degrees (negative spins ccw)
void turn_clockwise(oi_t *sensor_T, int degrees);


void bump_resL(oi_t *sensor_BL);



#endif /* MOVEMENT_H_ */
