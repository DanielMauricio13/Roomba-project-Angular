




#include "uart.h"
#include "timer.h"

volatile int uart_event;
volatile char uart_data;


void uart_init(void){
    SYSCTL_RCGCUART_R |= 0b10;      // enable clock UART1 (page 344)
    SYSCTL_RCGCGPIO_R |= 0b10;      // enable clock GPIOB (page 340)

    GPIO_PORTB_AFSEL_R |= 0b11;     //enable alternate function for PB0 and PB1
    GPIO_PORTB_PCTL_R &= ~0xFF;     //clear PCTL for PB0/1
    GPIO_PORTB_PCTL_R |= 0x11;      //assign PB0/1 to Rx and Tx (respectively)
    GPIO_PORTB_DEN_R |= 0x03;        // enables pb0 and pb1
    GPIO_PORTB_DIR_R &= ~0b1;        // sets pb0 as input
    GPIO_PORTB_DIR_R |= 0b10;        // sets pb1 as output


    //int UARTSysClk = 16E6;      //uart clock frequency
    //int ClkDiv = 16;            //16 by default

    //int int_brd = UARTSysClk / (ClkDiv * 115200);
    //int frac_brd = 0.8507 * 64 + 0.5;

    UART1_CTL_R &= 0b0;         //disable uart
    UART1_IBRD_R = 8;           //integer part of baud rate divisor
    UART1_FBRD_R = 44;          //fractional part of BRD
    UART1_LCRH_R = 0x60;        //8 bit word length tx/rx
    UART1_CC_R = 0b0;
    UART1_CTL_R |= 0b1;         //enable UART
    UART1_CTL_R |= 0x300;       //enable Rx and Tx ???

}

void uart_sendChar(char data){

   int i;
   int hasData = 0;

   for (i=0; i<1000; i++){
       if (!(UART1_FR_R & 0x20)){
           UART1_DR_R = data;
           hasData = 1;
           break;
       }
       timer_waitMillis(1);
   }
   if (!hasData){
       UART1_DR_R = '\0';
   }

    /*while (UART1_FR_R & 0x20){
        //wait until transmitter is not full
    }
    UART1_DR_R = data;*/


}

char uart_receive(void){

    int i;

    for (i=0; i<1000; i++){
       if (!(UART1_FR_R & 0x10)){
           return UART1_DR_R;
       }
       timer_waitMillis(1);
    }
    return '\0';

    /*while (UART1_FR_R & 0x10){
        //wait until receiver is not empty
    }
    return UART1_DR_R;*/

}

void uart_sendStr(const char *data){
	int readVal = 0;

	while (data[readVal] != '\0'){
	    uart_sendChar(data[readVal]);
	    readVal++;
	}
}



void uart_interrupt_init(){
    // Enables UART1 Rx
    UART1_IM_R |= 0b10000;

    // Find the NVIC enable register and bit responsible for UART1 in table 2-9
    // Note: NVIC register descriptions are found in chapter 3.4
    NVIC_EN0_R |= 0b1000000;

    // Find the vector number of UART1 in table 2-9 ! UART1 is 22 from vector number page 104
    IntRegister(INT_UART1, uart_interrupt_handler); //give the microcontroller the address of our interrupt handler - page 104 22 is the vector number

}

void uart_interrupt_handler(){


    if (UART1_MIS_R & 0b10000){//bit 12 detects if interrupt received


        uart_data = UART1_DR_R;
        uart_event = 1;


        UART1_ICR_R |= 0b10000; //write 1 to RXIC (bit 4) in uartICR register to clear UARTMIS
        UART1_ICR_R &= ~0b10000; //do I have to do this
    }
}









