/*
 * File:   interruptSetup.c
 * Author: Nathan Jourdain, Christian Vicaldo, Rodney, Del Rosario
 * Group: Group Number 5
 *
 * Created on October 22, 2025, 4:51 PM
 */


#include "xc.h"
#include "interruptSetup.h"

void InitInterrupt() {
    // Clear Interrupt Flags
    IFS1bits.CNIF = 0; 
    IFS0bits.T1IF = 0;
    IFS0bits.T2IF = 0;
    IFS0bits.T2IF = 0;
    
    // CN Interrupt Setup 
    IEC1bits.CNIE = 1; // Enable the CN interrupt
    CNEN1bits.CN0IE = 1; // Enable CN interrupt for RA4
    CNEN1bits.CN1IE = 1; // Enable CN interrupt for RB4
    CNEN2bits.CN23IE = 1; // Enable CN interrupt for RB7
    
    // Timer Interrupt Setup
    IEC0bits.T1IE = 1; // Enable Timer1 interrupt
    IEC0bits.T2IE = 1; // Enable Timer2 interrupt
    IEC0bits.T3IE = 1; // Enable Timer3 interrupt
    
    // Set Interrupt Priorities
    IPC4bits.CNIP = 6; // CN Interrupt has higher priority than timers
    IPC0bits.T1IP = 2; // Timer1 Interrupt Priority = 2
    IPC1bits.T2IP = 2; // Timer2 Interrupt Priority = 2
    IPC2bits.T3IP = 2; // Timer3 Interrupt Priority = 2
}

// ISR Definitions
uint8_t PBevent, T1event, T2event, T3event = 0; // global interrupt flags

uint32_t time = 0; //tracks the time passed since program start in 10ms increments
// Dont worry, program can run for over a month without any overflow issues

void __attribute__((interrupt, no_auto_psv)) _CNInterrupt(void) {
    IFS1bits.CNIF = 0; // Clear CN Interrupt Flag
    PBevent = 1;
}

void __attribute__((interrupt, no_auto_psv)) _T1Interrupt(void) {
    IFS0bits.T1IF = 0; // Clear Timer1 Interrupt Flag
    T1event = 1;
}

void __attribute__((interrupt, no_auto_psv)) _T2Interrupt(void) {
    IFS0bits.T2IF = 0; // Clear Timer2 Interrupt Flag
    T2event = 1;
}

void __attribute__((interrupt, no_auto_psv)) _T3Interrupt(void) {
    IFS0bits.T3IF = 0; // Clear Timer3 Interrupt Flag
    time++;
    T3event = 1;
}
