/*
* Copyright (c) 2014 Manolis Agkopian
* See the file LICENCE for copying permission.
*/

#include <xc.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "us-020.h"

#define _XTAL_FREQ 12000000

void us020_init (void) {
	TRIS_ECHO = 1;
	TRIS_ECHO2 = 1;
	
	TRIS_TRIGGER = 0;
	TRIGGER = 0;
	TRIS_TRIGGER2 = 0;
	TRIGGER2 = 0;

	CCP1CON = 0b00000101; // Capture mode, every rising edge
	T1CON = 0b00011000; // 1:2 Prescale
}

/*int us020_read (void) {
	int INCAPResult;
	float distance;

	PIR1bits.CCP1IF = 0; // No Capture Occurred
	TMR1H = 0; //Reset Timer1
	TMR1L = 0;
	PIR1bits.TMR1IF = 0; // No Timer1 Overflow Occurred
	T1CONbits.TMR1ON = 1; // Start Timer1

	TRIGGER = 1;

	while(!PIR1bits.CCP1IF);
	INCAPResult = (int) TMR1H; // Read high byte
	INCAPResult <<= 8;
	INCAPResult |= (int) TMR1L; // Read low byte

	TRIGGER = 0;

	distance = INCAPResult / 57.0 - 30;
	if (distance >= 50) {
		distance = 50 + ((distance - 50) * 0.695);
	}
	else {
		distance = 50 - ((50 - distance) * 0.649);
	}

	return (int) round(abs(distance - 2));
}*/

long unsigned int us020_read_1 (void) {
	unsigned long int time = 0;
	unsigned int ovf = 0;


	while(1) {
		if(ECHO == 0) {
			break;
		}
	}

	TRIGGER = 1;
	__delay_us(10);
	TRIGGER = 0;

	while(1) {
        if(ECHO == 1)
        {
            TMR1H = 0; //Reset Timer1
			TMR1L = 0;
			PIR1bits.TMR1IF = 0;
			T1CONbits.TMR1ON = 1; // Start Timer1
            while(1)
            {
				if (PIR1bits.TMR1IF == 1) {
					++ovf;
				}
                if(ECHO == 0){
					time |= (unsigned int) ovf;
					time <<= 16;
                    time = (unsigned int) TMR1H; // Read high byte
					time <<= 8;
					time |= (unsigned int) TMR1L; // Read low byte

                    return (long unsigned int) round(time / 87.0 - 2);
                }
            }
        }
    }
	return 0;
}

long unsigned int us020_read_2 (void) {
	unsigned long int time = 0;
	unsigned int ovf = 0;


	while(1) {
		if(ECHO2 == 0) {
			break;
		}
	}

	TRIGGER2 = 1;
	__delay_us(10);
	TRIGGER2 = 0;

	while(1) {
        if(ECHO2 == 1)
        {
            TMR1H = 0; //Reset Timer1
			TMR1L = 0;
			PIR1bits.TMR1IF = 0;
			T1CONbits.TMR1ON = 1; // Start Timer1
            while(1)
            {
				if (PIR1bits.TMR1IF == 1) {
					++ovf;
				}
                if(ECHO2 == 0){
					time |= (unsigned int) ovf;
					time <<= 16;
                    time = (unsigned int) TMR1H; // Read high byte
					time <<= 8;
					time |= (unsigned int) TMR1L; // Read low byte

                    return (long unsigned int) round(time / 87.0 - 2);
                }
            }
        }
    }
	return 0;
}