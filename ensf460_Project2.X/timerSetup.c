/*
 * File:   timerSetup.c
 * Author: Nathan Jourdain, Christian Vicaldo, Rodney, Del Rosario
 * Group: Group Number 5
 *
 * Created on October 23, 2025, 10:49 AM
 */

#include "xc.h"

void InitTimers() {
    // Note: Timer1 not initialized here since it is used for delay_ms()
    
    T2CONbits.T32 = 0; // operate timer 2 and 3 as 16 bit timers
    
    // Initialize Timer2 (1s interrupts)
    T2CONbits.TCKPS = 1; // set prescaler to 1:8
    T2CONbits.TCS = 0; // use internal clock
    T2CONbits.TSIDL = 0; //operate in idle mode
    
    PR2 = (uint16_t)(31250); // set the count value (1s interrupts)
    TMR2 = 0;
    
    // Initalize Timer3 (10ms interrupts)
    T3CONbits.TCKPS = 0; // set prescaler to 1:1
    T3CONbits.TCS = 0; // use internal clock
    T3CONbits.TSIDL = 0; // operate in idle mode
    
    PR3 = (uint16_t)(2500); // set the count value (10ms interrupts)
    TMR3 = 0;
}

void startTimer2() {
    TMR2 = 0;
    T2CONbits.TON = 1;
}

void stopTimer2() {
    T2CONbits.TON = 0;
}

void startTimer3() {
    TMR3 = 0;
    T3CONbits.TON = 1;
}

void stopTimer3() {
    T3CONbits.TON = 0;
}
