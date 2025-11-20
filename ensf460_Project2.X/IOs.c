/*
 * File:   IOs.c
 * Author: Nathan Jourdain, Christian Vicaldo, Rodney, Del Rosario
 * Group: Group Number 5
 *
 * Created on September 26, 2025, 3:19 PM
 */

#include "xc.h"
#include "IOs.h"
#include "UART2.h"
#include "timerSetup.h"
#include "delay_ms.h"
#include "button.h"

Button_t PB1 = {0}, PB2 = {0}, PB3 = {0};

extern uint32_t time;
extern uint8_t T2event;
extern uint8_t PBevent;
extern uint8_t alarmFlag;

#define HOLD_TIME 3000 // length of time (ms) for button to be considered a long press
#define COMBO_TIME 100 // buttons must be pressed within 100ms of eachother to be counted as together
#define REPEAT_DELAY 300 // time between repeating actions

void InitIO() {
    
    AD1PCFG = 0xFFFF; /* keep this line as it sets I/O pins that can also be analog to be digital */
    
    TRISBbits.TRISB9 = 0; // LED1 Setup
    LATBbits.LATB9 = 0; 
    
    TRISAbits.TRISA6 = 0; // LED2 Setup
    LATAbits.LATA6 = 0;

    TRISBbits.TRISB7 = 1; // PB1 Setup
    CNPU2bits.CN23PUE = 1; // Enable Pull-Up for PB1
    CNEN2bits.CN23IE = 1; // Enable CN Interrupt for PB1
    
    TRISAbits.TRISA4 = 1; // PB2 Setup
    CNPU1bits.CN0PUE = 1; // Enable Pull-Up for PB2
    CNEN1bits.CN0IE = 1; // Enable CN Interrupt for PB2

    TRISBbits.TRISB4 = 1; // PB3 Setup
    CNPU1bits.CN1PUE = 1; // Enable Pull-Up for PB3
    CNEN1bits.CN1IE = 1; // Enable CN Interrupt for PB3

    TRISAbits.TRISA3 = 1; // Potentiometer Setup
    AD1PCFGbits.PCFG5 = 0; // set RA3 (AN5) to analog
    
}

void updateIOstate() {
    do {
        delay_ms(30); // 30ms delay for debounce
        PB1.isPressed = !PORTBbits.RB7;
        PB2.isPressed = !PORTBbits.RB4;
        PB3.isPressed = !PORTAbits.RA4;
        
        Button_t* btns[3] = { &PB1, &PB2, &PB3 };

        // Update each button individually
        for (int i = 0; i < 3; i++) {
            Button_t* b = btns[i];
            b->holdTime = time - b->timePressed;

            // Moment the button is pressed
            if (b->isPressed && !b->prevState) {
                b->timePressed = time;
                b->timeReleased = 0;
                b->wasHeld = 0;
                b->wasShortPressed = 0;
                b->holdTime = 0;
            }
            // Held long enough
            else if (b->isPressed && (b->holdTime >= HOLD_TIME) && !b->wasHeld) {
                b->wasHeld = 1;
                b->wasShortPressed = 0;
            }
            // Released before hold flag
            else if (!b->isPressed && b->prevState && !b->wasHeld) {
                b->wasShortPressed = 1;
                b->wasHeld = 0;
                b->timeReleased = time;
                b->holdTime = 0;
            }
            // Released after hold
            else if (!b->isPressed && b->prevState && b->wasHeld) {
                b->holdTime = 0;
                b->wasShortPressed = 0;
            }

            b->prevState = b->isPressed;
        }       
   
//        handleIOstate();
        
    } while(PB1.isPressed | PB2.isPressed | PB3.isPressed);
        
    // clear old flags
    PB1.wasHeld = PB1.wasShortPressed = 0;
    PB2.wasHeld = PB2.wasShortPressed = 0;
    PB3.wasHeld = PB3.wasShortPressed = 0;
}
