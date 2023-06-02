

#include "sensor.h"
volatile int overflow_event;

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










void ping_init(void){
    SYSCTL_RCGCTIMER_R |= 0b1000;   // enable clock TIMER3

    SYSCTL_RCGCGPIO_R |= 0b10;      // enable clock GPIOB


    GPIO_PORTB_DEN_R |= 0b1000;     //enable pb3

    GPIO_PORTB_PCTL_R &= ~0xF000;   //set pb3 to timer
    GPIO_PORTB_PCTL_R |= 0x7000;

    TIMER3_CTL_R &= ~0x100;     //turn off timer B (timer 3)
    TIMER3_CFG_R |= 0x4;        //sets register according to datasheet for input edge time mode
    TIMER3_TBMR_R &= ~0xFFF;
    TIMER3_TBMR_R |= 0b111;     //sets edge time capture mode
    TIMER3_CTL_R |= 0xC00;      //positive and negative edge trigger

    TIMER3_TBILR_R = 0xFFF; //GPTMTAILR
    TIMER3_TBPR_R = 0xFF;

    TIMER3_CTL_R |= 0x100;      //re-enable timer B

}


float ping_read(void){
    timer_waitMillis(100); //wait a bit for sensor to stop moving
    //char puttyIn[70] = "";
    int p1, p2;
    //TIMER3_IMR_R &= ~0x400; //disables ISR

    GPIO_PORTB_AFSEL_R &= ~0b1000; //disables alternate function for PB3
    GPIO_PORTB_DIR_R |= 0b1000;     //pb3 set to output

    /* send trigger signal */
    GPIO_PORTB_DATA_R |= 0b1000; //sets pb3 to high
    timer_waitMillis(1);
    GPIO_PORTB_DATA_R &= ~0b1000; //sets pb3 to low

    GPIO_PORTB_AFSEL_R |= 0b1000;   //enables alternate function for PB3
    GPIO_PORTB_DIR_R &= ~0b1000; //pb3 set to input


    TIMER3_ICR_R |= 0x400; //clear timer interrupt
    //TIMER3_IMR_R |= 0x400; //reenables ISR

    while (!(TIMER3_RIS_R & 0x400)){
        //wait until change detected
    }
    p1 = TIMER3_TBR_R & 0xFFFFFF; //first value (first 24 bits only)


    TIMER3_ICR_R |= 0x400; //clear timer interrupt
    while (!(TIMER3_RIS_R & 0x400)){
        //wait until change detected
    }
    p2 = TIMER3_TBR_R & 0xFFFFFF; //2d value (< than 1st unless overflow) (first 24 bits only)
    TIMER3_ICR_R |= 0x400; //clear timer interrupt



    //sprintf(puttyIn, "val1: %d\tval2: %d\t\tdiff: %d\r\n", p1, p2, p1-p2);
    //uart_sendStr(puttyIn);



    if (p1<p2){ //if overflow
        //p2 = 16777215 - (p2-p1); //maximum value minus difference between p2 and p1
        overflow_event = 1;

        return ping_read(); //read it again
    }
    //else{
    //    p2 = p1-p2;
    //}

    //p2 = ((p1-p2) & 0xFFFFFF);

    return (p1-p2)*0.0010625;
}













