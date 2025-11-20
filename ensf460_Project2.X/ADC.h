#ifndef ADC_H
#define ADC_H

#include <xc.h>

// Function declarations
void InitADC(void);

uint16_t do_ADC(uint8_t return_as_percent);

#endif // ADC_H