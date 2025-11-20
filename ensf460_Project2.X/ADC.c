/*
 * File:   ADC.c
 * Author: Nathan Jourdain, Christian Vicaldo, Rodney, Del Rosario
 * Group: Group Number 5
 *
 * Created on October 24, 2025, 3:31 PM
 */


#include "xc.h"
#include "ADC.h"

void InitADC(void) {
    AD1PCFG = 0xFFFF; /* keep this line as it sets I/O pins that can also be analog to be digital */
    
    AD1CON1bits.FORM = 0b00;       // Integer output (no formatting)
    AD1CON1bits.SSRC = 0b111;       // Manual conversion trigger
    AD1CON1bits.ASAM = 0;       // Sampling begins when SAMP = 1

    AD1CON2bits.VCFG = 0b000; // use AVDD and AVSS
    AD1CON2bits.SMPI = 0b0000;
    AD1CON2bits.CSCNA = 0; 
    AD1CON2bits.SMPI = 0;
    AD1CON2bits.BUFM = 0;
    AD1CON2bits.ALTS = 0;
    
    AD1CON3bits.ADCS = 0b111111;
    AD1CON3bits.ADRC = 0; // use internal clock
    AD1CON3bits.SAMC = 0b11111;
    
    AD1CHSbits.CH0NA = 0;
    AD1CHSbits.CH0NB = 0;
    AD1CHSbits.CH0SA = 0b0101;
    AD1CHSbits.CH0SB = 0b0101;   
    
    AD1CSSL = 0;
}

uint16_t do_ADC(uint8_t return_as_percent) {
    // return_as_percent = 1 -> return the ADC reading as a percentage
    // else return the reading 0-1023
    
    uint16_t ADCvalue;
    
    AD1CON1bits.ADON = 1; // turn on the ADC module
    AD1CON1bits.SAMP = 1; // start sampling
    
    while(AD1CON1bits.DONE==0)
        {}
    
    ADCvalue = ADC1BUF0;
    AD1CON1bits.SAMP = 0; // Stop sampling
    AD1CON1bits.ADON = 0; // Turn off ADC
    
    if(return_as_percent == 1) {
        return (ADCvalue / 1023.0) * 100;
    }
    else {
        return (ADCvalue);
    }
}
