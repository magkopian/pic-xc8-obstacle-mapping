/* 
 * File:   us-020.c
 * Author: Manolis Agkopian
 *
 * Created on 17 Ιανουάριος 2014, 1:30 πμ
 */

#include <xc.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "us-020.h"

void us020_init (void) {
	TRIS_ECHO = 1;
	TRIS_TRIGGER = 0;
	TRIGGER = 0;

	CCP1CON = 0b00000101; // Capture mode, every rising edge
	T1CON = 0b00011000; // 1:2 Prescale
}

int us020_read (void) {
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

	return (int) round(distance);
}