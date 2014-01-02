/* 
 * File:   sn754410.c
 * Author: Manolis Agkopian
 *
 * Created on 2 Ιανουάριος 2014, 8:33 μμ
 */

#include <xc.h>
#include <plib/pwm.h>
#include <stdio.h>
#include <stdlib.h>
#include "sn754410.h"

#define _XTAL_FREQ 12000000

unsigned char last_action = MOVE_STOP;

void sn754410_init(void) {
	TRIS_EN12 = TRIS_EN34 = 0; // Enable control pins are outputs
	EN12 = EN34 = 0;

	// 1A, 2A, 3A and 4A H-Brige input control pins are outputs
	TRIS_A4 = 0; // P1A (4A)
	TRIS_A2 = 0; // P1B (2A)
	TRIS_A3 = 0; // P1C (3A)
	TRIS_A1 = 0; // P1D (1A)

	A3 = A4 = 0; // Only A3 and A2 are IO pins

	OpenEPWM1(0xFF); // Configure PWM module and initialize PWM period
	SetOutputEPWM1(FULL_OUT_FWD, PWM_MODE_1);
	SetDCEPWM1(1023); // Set the duty cycle
}

void sn754410_fwd(void) {
	EN12 = EN34 = 1;
	SetOutputEPWM1(FULL_OUT_FWD, PWM_MODE_1);
	A3 = 0;
	A4 = 1;

	last_action = MOVE_FORWARD;
}

void sn754410_rev(void) {
	EN12 = EN34 = 1;
	SetOutputEPWM1(FULL_OUT_REV, PWM_MODE_1);
	A4 = 0;
	A3 = 1;

	last_action = MOVE_REVERSE;
}

void sn754410_trnl(void) {
	EN12 = EN34 = 1;
	SetOutputEPWM1(FULL_OUT_FWD, PWM_MODE_1);
	A4 = 0;
	A3 = 1;

	last_action = MOVE_LEFT;
}

void sn754410_trnr(void) {
	EN12 = EN34 = 1;
	SetOutputEPWM1(FULL_OUT_REV, PWM_MODE_1);
	A3 = 0;
	A4 = 1;

	last_action = MOVE_RIGHT;
}

void sn754410_brk(void) {
	// Break motors
	if (last_action == MOVE_FORWARD) {
		sn754410_rev();
	}
	else if (last_action == MOVE_REVERSE) {
		sn754410_fwd();
	}
	else if (last_action == MOVE_LEFT) {
		sn754410_trnr();
	}
	else if (last_action == MOVE_RIGHT) {
		sn754410_trnl();
	}
	for (int i = 0; i < 7; ++i) __delay_ms(10);

	// Deactivate motors
	EN12 = EN34 = 0;
	A3 = A4 = 0;

	last_action = MOVE_STOP;
}