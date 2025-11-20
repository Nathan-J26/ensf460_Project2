#ifndef BUTTON_H
#define BUTTON_H

#include <xc.h>

typedef struct {
    uint8_t isPressed;
    uint8_t prevState;
    uint32_t timePressed;
    uint32_t timeReleased;
    uint32_t holdTime;
    uint8_t wasHeld;
    uint8_t wasShortPressed;
} Button_t;

#endif // BUTTON_H