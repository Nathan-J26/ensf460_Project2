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

typedef enum {
    STATE_Off,
    STATE_OnSolid,
    STATE_BlinkFullbright,
    STATE_Blink,
    STATE_Record
} state_t;

Button_t PB1 = {0}, PB2 = {0}, PB3 = {0};

state_t state = STATE_Off;

volatile uint8_t activeLED = 0;
extern uint32_t time;
extern uint8_t T2event;
extern uint8_t PBevent;
extern uint8_t alarmFlag;
extern volatile uint8_t doBlink;

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
//                Disp2String("LONG PRESS\n\r");
                b->wasHeld = 1;
                b->wasShortPressed = 0;
            }
            // Released before hold flag
            else if (!b->isPressed && b->prevState && !b->wasHeld) {
//                Disp2String("SHORT PRESS\n\r");
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
   
        handleIOstate();
        
    } while(PB1.isPressed | PB2.isPressed | PB3.isPressed);
        
    // clear old flags
    PB1.wasHeld = PB1.wasShortPressed = 0;
    PB2.wasHeld = PB2.wasShortPressed = 0;
    PB3.wasHeld = PB3.wasShortPressed = 0;
}


void handleIOstate() {
    switch(state) {
        case STATE_Off: {
            if(PB1.wasShortPressed && !PB2.isPressed && !PB3.isPressed) {
                Disp2String("Transition to ON SOLID\n\r");
                activeLED = 1;
                state = STATE_OnSolid;
                doBlink = 0;
                break;
            }
            else if (!PB1.isPressed && PB2.wasShortPressed && !PB3.isPressed) {
                Disp2String("Transition to Blink at 100%\n\r");
                state = STATE_BlinkFullbright;
                activeLED = 1;
                doBlink = 1;
                break;
            }
            break;
        }
        
        case STATE_OnSolid: {
            
            if(PB1.isPressed && !PB2.isPressed && !PB3.isPressed) {
                if(PB1.wasHeld) {
                    if(activeLED == 1) {
                        activeLED = 2;
                        Disp2String("Setting LED2\n\r");
                        delay_ms(500);
                    }
                    else {
                        Disp2String("Setting LED1\n\r");
                        activeLED = 1;
                        delay_ms(500);
                    }
                }
            }
            else if(PB1.wasShortPressed && !PB2.isPressed && !PB3.isPressed) {
                Disp2String("Transition to OFF\n\r");
                state = STATE_Off;
                PORTBbits.RB9 = 0;
                PORTAbits.RA6 = 0;
                activeLED = 0;
            }
            else if(!PB1.isPressed && PB2.wasShortPressed && !PB3.isPressed) {
                Disp2String("Transition to Blink\n\r");
                state = STATE_Blink;
                doBlink = 1;
            }
//            else if(!PB1.isPressed && !PB2.isPressed && PB3.wasShortPressed) {
//                Disp2String("Begin recording\n\r");
//                state = STATE_Record;
//                break;
//            }
            break;
        }
        
        case STATE_BlinkFullbright: {
            if(PB1.isPressed && !PB2.isPressed && !PB3.isPressed) {
                if(PB1.wasHeld) {
                    if(activeLED == 1) {
                        activeLED = 2;
                        Disp2String("Setting LED2\n\r");
                        delay_ms(500);
                    }
                    else {
                        Disp2String("Setting LED1\n\r");
                        activeLED = 1;
                        delay_ms(500);
                    }
                }
            }
            else if((PB1.wasShortPressed || PB2.wasShortPressed) && !PB3.isPressed) {
                Disp2String("Transition to OFF\n\r");
                state = STATE_Off;
                doBlink = 0;
                activeLED = 0;
                PORTBbits.RB9 = 0;
                PORTAbits.RA6 = 0;
                break;
            } 
//            else if(!PB1.isPressed && !PB2.isPressed && PB3.wasShortPressed) {
//                Disp2String("Begin recording\n\r");
//                state = STATE_Record;
//                break;
//            }
        }
        
        case STATE_Blink: {
            if(PB1.isPressed && !PB2.isPressed && !PB3.isPressed) {
                if(PB1.wasHeld) {
                    if(activeLED == 1) {
                        activeLED = 2;
                        Disp2String("Setting LED2\n\r");
                        delay_ms(500);
                    }
                    else {
                        Disp2String("Setting LED1\n\r");
                        activeLED = 1;
                        delay_ms(500);
                    }
                }
            }
            else if((PB1.wasShortPressed || PB2.wasShortPressed) && !PB3.isPressed) {
                Disp2String("Transition to ON\n\r");
                state = STATE_OnSolid;
                doBlink = 0;

                
                break;
            } 
//            else if(!PB1.isPressed && !PB2.isPressed && PB3.wasShortPressed) {
//                Disp2String("Begin recording\n\r");
//                state = STATE_Record;
//                break;
//            }
        }
        
//        case STATE_Record: {
//            if(PB1.wasShortPressed && !PB2.isPressed && PB3.isPressed) {
//                Disp2String("Recording Finished. Transition to OFF\n\r");
//                state = STATE_Off;
//                break;
//            }
//            else if(!PB1.isPressed && !PB2.isPressed && PB3.wasShortPressed) {
//                Disp2String("Recording Finished. Transition to ON\n\r");
//                if(doBlink) {
//                    state = STATE_Blink;
//                }
//                else {
//                    state = STATE_OnSolid;
//                }
//                break;
//            }
//        }
    }
}