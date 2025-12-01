/*
 * File:   interruptSetup.c
 * Author: Nathan Jourdain, Christian Vicaldo, Rodney, Del Rosario
 * Group: Group Number 5
 *
 * Created on October 22, 2025, 4:51 PM
 */


#include "xc.h"
#include "interruptSetup.h"
#include "pwm.h"

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
volatile uint8_t ledBlinkState = 1;
volatile uint8_t counter = 0;
volatile uint8_t blinkFlag = 1;
volatile uint8_t doBlink = 0;
uint32_t time = 0; //tracks the time passed since program start
extern volatile uint16_t pwm_threshold;
extern volatile uint8_t activeLED;

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
    time += 10;
    counter += 1;
    if(counter >= 50) {
        counter = 0;
        if(doBlink) {
            blinkFlag ^= 1;
        }
    }
    T2event = 1;
}

void __attribute__((interrupt, no_auto_psv)) _T3Interrupt(void) {
    IFS0bits.T3IF = 0;
    T3event = 1;
    
    static uint16_t pwm_counter = 0;

    pwm_counter++;
    if (pwm_counter >= PWM_PERIOD) pwm_counter = 0;

    uint8_t blink_enable = (doBlink ? blinkFlag : 1);
    
    if(activeLED != 0) {
        if(activeLED == 1) {
            PORTBbits.RB9 = ((pwm_counter < pwm_threshold) && blink_enable);
            PORTAbits.RA6 = 0;
        }
        else if(activeLED == 2) {
            PORTBbits.RB9 = 0;
            PORTAbits.RA6 = ((pwm_counter < pwm_threshold) && blink_enable);
        }
    }
    else {
        PORTBbits.RB9 = 0;
        PORTAbits.RA6 = 0;
    }
 
}
