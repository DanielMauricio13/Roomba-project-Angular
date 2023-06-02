

#include "sensor.h"


void laser_init(void){
    SYSCTL_RCGCADC_R |= 0b1;      // enable clock ADC0 (page 352)

    SYSCTL_RCGCGPIO_R |= 0b10;      // enable clock GPIOB


    //PB4 laser (ADC)

    GPIO_PORTB_AFSEL_R |= 0x10;     //enables alternate function for PB4

    GPIO_PORTB_DEN_R &= ~0x10;   //disables PB4
    GPIO_PORTB_DIR_R &= ~0x10;   //PB4 input
    GPIO_PORTB_AMSEL_R |= 0x10;     //enables ADC for PB4

    GPIO_PORTB_ADCCTL_R = 0;

    //ADC0_PC_R &= ~0xF;
    //ADC0_PC_R |= 0x1;

    ADC0_ACTSS_R &= ~0b1;       //disabled ADC
    ADC0_EMUX_R &= ~0xF;         //usedefault processor
    ADC0_SSMUX0_R |= 0xA;       //0xA indicates output is AIN10
    ADC0_SSCTL0_R &= ~0xF;
    ADC0_SSCTL0_R |= 0b110;     //first sample is last in sequence (also enables RIS write)
    ADC0_ACTSS_R |= 0b1;        //enables ADC


}



int laser_read(void){
    ADC0_PSSI_R |= 0b1;         //begins sampling apparently
    while (!(ADC0_RIS_R & 0b1)){

    }
    ADC0_ISC_R = 0b1;
    return ADC0_SSFIFO0_R;
}



