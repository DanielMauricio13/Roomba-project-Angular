/*
*
*   uart.h
*
*   Used to set up the UART
*   uses UART1 at 115200
*
*
*   @author Dane Larson
*   @date 07/18/2016
*   Phillip Jones updated 9/2019, removed WiFi.h
*/

#ifndef UART_H_
#define UART_H_

#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include "timer.h"
#include "lcd.h"
#include <inc/tm4c123gh6pm.h>
#include "driverlib/interrupt.h"

// Initialize the UART to communciate between CyBot and PuTTy
void uart_init(void);


// Send a byte to putty display
void uart_sendChar(char data);

// receive a user input from putty
char uart_receive(void);


// Send a string over the UART from CyBot and PuTTy (uses sendChar method)
void uart_sendStr(const char *data);


void uart_interrupt_init();


void uart_interrupt_handler();


#endif /* UART_H_ */





