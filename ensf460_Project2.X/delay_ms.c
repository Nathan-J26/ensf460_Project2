/*
 * File:   TimeDelay.c
 * Author: Nathan Jourdain, Christian Vicaldo, Rodney, Del Rosario
 * Group: Group Number 5
 *
 * Created on September 30, 2025, 3:46 PM
 */


#include "xc.h"
#include "delay_ms.h"

extern uint8_t T1event;

void delay_ms(uint16_t time_ms) {
    
    //T1CON config
    T1CONbits.TCKPS = 3; // set prescaler to 1:256
    T1CONbits.TCS = 0; // use internal clock
    T1CONbits.TSIDL = 0; //operate in idle mode
    
    PR1 = (uint16_t)(0.9765625 * time_ms); // set the count value
    TMR1 = 0;
    T1CONbits.TON = 1;
    
    while(!T1event){
        Idle();
    }
    T1event = 0;
    
    T1CONbits.TON = 0; // stop the timer once interrupted
}
