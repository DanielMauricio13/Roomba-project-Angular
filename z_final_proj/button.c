//The buttons are on PORTE 3:0
// GPIO_PORTE_DATA_R -- Name of the memory mapped register for GPIO Port E, 
// which is connected to the push buttons
#include "button.h"


/**
 * Initialize PORTE and configure bits 0-3 to be used as inputs for the buttons.
 */
void button_init() {
	static uint8_t initialized = 0;

	//Check if already initialized
	if(initialized){
		return;
	}
	
	// 1) Turn on PORTE system clock, do not modify other clock enables
	SYSCTL_RCGCGPIO_R |= 0b00010000;

	// 2) Set the buttons as inputs, do not modify other PORTE wires
	GPIO_PORTE_DIR_R &= 0b00000000;
	
	// 3) Enable digital functionality for button inputs, 
	//    do not modify other PORTE enables
	GPIO_PORTE_DEN_R |= 0b11111111;

	
	initialized = 1;
}



/**
 * Returns the position of the rightmost button being pushed.
 * @return the position of the righttmost button being pushed. 4 is the rightmost button, 1 is the leftmost button.  0 indicates no button being pressed
 */
uint8_t button_getButton() {

    if ((GPIO_PORTE_DATA_R & 0b00001000) == 0b00000000){//b4
        return 4;
    }
    if ((GPIO_PORTE_DATA_R & 0b00000100) == 0b00000000){//b3
            return 3;
        }
    if ((GPIO_PORTE_DATA_R & 0b00000010) == 0b00000000){//b2
            return 2;
        }
    if ((GPIO_PORTE_DATA_R & 0b00000001) == 0b00000000){//b1
            return 1;
        }
    return 0;
}





