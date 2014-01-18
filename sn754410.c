/* 
 * File:   sn754410.c
 * Author: Manolis Agkopian
 *
 * Created on 2 Ιανουάριος 2014, 8:33 μμ
 */

#include <xc.h>
#include <math.h>
#include <plib/pwm.h>
#include <plib/delays.h>
#include <stdio.h>
#include <stdlib.h>
#include "sn754410.h"
#include "hmc5883l.h"

#define _XTAL_FREQ 12000000

unsigned char last_action = MOVE_STOP;
unsigned int turn_to = DEG_0;

extern double angle;

void sn754410_init() {
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

void sn754410_fwd() {
	EN12 = EN34 = 1;
	SetOutputEPWM1(FULL_OUT_FWD, PWM_MODE_1);
	A3 = 0;
	A4 = 1;

	last_action = MOVE_FORWARD;
}

void sn754410_rev() {
	EN12 = EN34 = 1;
	SetOutputEPWM1(FULL_OUT_REV, PWM_MODE_1);
	A4 = 0;
	A3 = 1;

	last_action = MOVE_REVERSE;
}

void sn754410_trnl() {
	EN12 = EN34 = 1;
	SetOutputEPWM1(FULL_OUT_FWD, PWM_MODE_1);
	A4 = 0;
	A3 = 1;

	last_action = MOVE_LEFT;
}

void sn754410_trnr() {
	EN12 = EN34 = 1;
	SetOutputEPWM1(FULL_OUT_REV, PWM_MODE_1);
	A3 = 0;
	A4 = 1;

	last_action = MOVE_RIGHT;
}

void sn754410_brk() {
	// Break motors
	if (last_action == MOVE_FORWARD) {
		sn754410_rev(angle);
	}
	else if (last_action == MOVE_REVERSE) {
		sn754410_fwd(angle);
	}
	else if (last_action == MOVE_LEFT) {
		sn754410_trnr(angle);
	}
	else if (last_action == MOVE_RIGHT) {
		sn754410_trnl(angle);
	}
	for (int i = 0; i < 7; ++i) __delay_ms(10);

	// Deactivate motors
	EN12 = EN34 = 0;
	A3 = A4 = 0;

	last_action = MOVE_STOP;
}

int sn754410_turn_to(unsigned int trn_angle) {
		if (trn_angle == DEG_0) {
			turn_to = DEG_0;

			if (angle > DEG_180) {
				sn754410_trnr();
			}
			else {
				sn754410_trnl();
			}
			return 0;
		}
		else if (trn_angle == DEG_90) {
			turn_to = DEG_90;

			if (angle > DEG_270) {
				sn754410_trnr();
			}
			else {
				sn754410_trnl();
			}
			return 0;
		}
		else if (trn_angle == DEG_180) {
			turn_to = DEG_180;

			if (angle < DEG_180) {
				sn754410_trnr();
			}
			else {
				sn754410_trnl();
			}
			return 0;
		}
		else if (trn_angle == DEG_270) {
			turn_to = DEG_270;

			if (angle > DEG_90) {
				sn754410_trnr();
			}
			else {
				sn754410_trnl();
			}
			return 0;
		}
		else {
			return -1;
		}
}

void sn754410_test_turn_to(void) {
	int x = 0, y = 0, z = 0;

	if (last_action == MOVE_LEFT || last_action == MOVE_RIGHT) {
		//EN12 = EN34 = 0;
		//A3 = A4 = 0;

		// Get x and y from HMC5883L
		hmc5883l_read(&x, &y, &z);

		// Convert to degrees
		angle = atan2((double) y, (double) x) * (180.0 / 3.14159265) + 180.0;

		if (!sn754410_break_if(turn_to)) {
			if (last_action == MOVE_LEFT) {
				sn754410_trnl();
			}
			else if (last_action == MOVE_RIGHT) {
				sn754410_trnr();
			}
		}
	}
}

void sn754410_test_move_fwd(void) {
	int x = 0, y = 0, z = 0;

	if (last_action == MOVE_FORWARD) {
		//EN12 = EN34 = 0;
		//A3 = A4 = 0;

		// Get x and y from HMC5883L
		hmc5883l_read(&x, &y, &z);

		// Convert to degrees
		angle = atan2((double) y, (double) x) * (180.0 / 3.14159265) + 180.0;


		if (turn_to == DEG_0 && angle < 180) {
			sn754410_trnl();
		}
		else if (turn_to == DEG_0 && angle > 180) {
			sn754410_trnr();
		}
		else if (angle > turn_to) {
			sn754410_trnl();
		}
		else if (angle < turn_to) {
			sn754410_trnr();
		}
		__delay_ms(2);
		sn754410_fwd();
	}
}

int sn754410_break_if(double brk_angle) {
	if (last_action == MOVE_LEFT) {
		if (brk_angle + SSE - TOLERANCE >= 360) {
			if (angle >= brk_angle - TOLERANCE + SSE - 360 && angle <= brk_angle + TOLERANCE + SSE - 360) {
				sn754410_brk();
				return 1;
			}

			if (angle <= brk_angle - TOLERANCE + SSE - 2 + 360 && angle >= brk_angle - TOLERANCE + SSE - 80 + 360) {
				sn754410_trnr();
			}
		}
		else {
			if (angle >= brk_angle - TOLERANCE + SSE && angle <= brk_angle + TOLERANCE + SSE) {
				sn754410_brk();
				return 1;
			}

			if (angle <= brk_angle - TOLERANCE + SSE - 2 && angle >= brk_angle - TOLERANCE + SSE - 80) {
				sn754410_trnr();
			}
		}
	}
	else if (last_action == MOVE_RIGHT) {
		if (brk_angle - SSE + TOLERANCE <= 0) {
			if (angle <= brk_angle + TOLERANCE - SSE + 360 && angle >= brk_angle - TOLERANCE - SSE + 360) {
				sn754410_brk();
				return 1;
			}

			if (angle >= brk_angle + TOLERANCE - SSE + 2 - 360 && angle <= brk_angle + TOLERANCE - SSE + 80 - 360) {
				sn754410_trnl();
			}
		}
		else {
			if (angle <= brk_angle + TOLERANCE - SSE && angle >= brk_angle - TOLERANCE - SSE) {
				sn754410_brk();
				return 1;
			}

			if (angle >= brk_angle + TOLERANCE - SSE + 2 && angle <= brk_angle + TOLERANCE - SSE + 80) {
				sn754410_trnl();
			}
		}
	}
	return 0;
}