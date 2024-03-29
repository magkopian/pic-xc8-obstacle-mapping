/*
* Copyright (c) 2014 Manolis Agkopian
* See the file LICENCE for copying permission.
*/

#include <xc.h>
#include <plib/delays.h>
#include <plib/usart.h>
#include <plib/i2c.h>
#include <plib/pwm.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "config_bits.h"
#include "hmc5883l.h"
#include "sn754410.h"
#include "us-020.h"

#define _XTAL_FREQ 12000000

#define STEP_IDLE 0x00
#define STEP_CALIB 0x33
#define STEP_SEARCH 0x99
#define STEP_FOUND_OBS 0x77
#define STEP_TURN 0xAA
#define STEP_FOWARD_WIDTH 0x85
#define STEP_FOUND_OBS_ON_W 0xDD

#define ROUTE_0 0x00
#define ROUTE_90 0x33
#define ROUTE_180 0xDD
#define ROUTE_270 0xF0

extern double angle;
extern unsigned char last_action;

long unsigned int distance1 = 9999;
long unsigned int distance2 = 9999;

unsigned int step = STEP_IDLE;
unsigned int route = ROUTE_0;
unsigned char turn_0_or_180 = 1;
unsigned char obs_turn_cnt = 0;
unsigned char f_w_cnt = 0;

void main(void) {
	unsigned char config;
    unsigned int spbrg;

	CloseUSART();
	CloseI2C();
	CloseEPWM1();

    TRISC6 = 0;
    TRISC7 = 1;

	//configure USART
    config = USART_TX_INT_OFF | USART_RX_INT_OFF | USART_ASYNCH_MODE | USART_EIGHT_BIT |
    USART_CONT_RX | USART_BRGH_HIGH | USART_ADDEN_OFF;

	//Bitrate = Fosc / (16 * (spbrg + 1))
    spbrg = 77; // About 9615 @ 12MHz (9600bps)

	OpenUSART(config, spbrg);

	//Only by doing this the usart library works correctly
    TX9 = 0;
	SYNC = 0;

	hmc5883l_init();
	sn754410_init();
	us020_init();

	// Initialize Timer 0
	T0CON = 0b01000111; // 8bit, 1:256 prescaler
	TMR0L = 0;
	TMR0H = 0;
	INTCONbits.T0IF = 0;
	T0CONbits.TMR0ON = 1;

	// Initialize Timer 3
	T3CON = 0b00010000; // 16bit, 1:2 prescaler
	TMR3L = 0;
	TMR3H = 0;
	PIR2bits.TMR3IF = 0;
	T3CONbits.TMR3ON = 1;

	for (;;) {
		// Check for Timer 0 overflow (5.46ms)
		if (INTCONbits.T0IF) {
			T0CONbits.TMR0ON = 0;
			sn754410_test_turn_to();
			TMR0L = 0;
			TMR0H = 0;
			INTCONbits.T0IF = 0;
			T0CONbits.TMR0ON = 1;
		}

		// Check for Timer 3 overflow (174.76ms)
		if (PIR2bits.TMR3IF) {
			T3CONbits.TMR3ON = 0;

			sn754410_test_move_fwd();
			if (step == STEP_FOWARD_WIDTH && last_action == MOVE_FORWARD && f_w_cnt++ > 7) { // 174.76ms * 6 = 1048.56ms
				sn754410_brk();
				f_w_cnt = 0;
			}

			TMR3L = 0;
			TMR3H = 0;
			PIR2bits.TMR3IF = 0;
			T3CONbits.TMR3ON = 1;
		}

		char buff[20];

		if ( ( (distance1 = us020_read_1()) <= 12 || (distance1 = us020_read_2()) <= 12 ) && step == STEP_SEARCH && last_action == MOVE_FORWARD ) {
			sn754410_brk();
			if (distance1 < distance2) {
				sprintf(buff, "ob;%d\r\n", distance1);
			}
			else {
				sprintf(buff, "ob;%d\r\n", distance2);
			}
			putsUSART(buff);
			step = STEP_FOUND_OBS;
		}
		else if ( ( distance1 <= 13 || distance2 <= 13 ) && step == STEP_FOWARD_WIDTH && last_action == MOVE_FORWARD ) {
			sn754410_brk();
			if (distance1 < distance2) {
				sprintf(buff, "ob;%d\r\n", distance1);
			}
			else {
				sprintf(buff, "ob;%d\r\n", distance2);
			}
			putsUSART(buff);
			step = STEP_FOUND_OBS_ON_W;
		}
		else {
			if (step == STEP_FOWARD_WIDTH) {
				putrsUSART("width\r\n");
			}
			else if (step == STEP_SEARCH) {
				if (distance1 < distance2) {
					sprintf(buff, "fw;%d\r\n", distance1);
				}
				else {
					sprintf(buff, "fw;%d\r\n", distance2);
				}
				putsUSART(buff);
			}
		}

		if ( (step == STEP_CALIB || (step == STEP_TURN && (route == ROUTE_0 || route == ROUTE_180) ) ) && last_action == MOVE_STOP ) {
			sn754410_fwd();
			step = STEP_SEARCH;
		}
		else if ((step == STEP_TURN && route == ROUTE_90) && last_action == MOVE_STOP) {
			sn754410_fwd();
			step = STEP_FOWARD_WIDTH;
		}
		else if ((step == STEP_FOWARD_WIDTH && turn_0_or_180) && last_action == MOVE_STOP) {
			sn754410_turn_to(DEG_0);
			putrsUSART("turn_0\r\n");
			step = STEP_TURN;
			route = ROUTE_0;
			turn_0_or_180 = 0;
		}
		else if ((step == STEP_FOWARD_WIDTH && !turn_0_or_180) && last_action == MOVE_STOP) {
			sn754410_turn_to(DEG_180);
			putrsUSART("turn_180\r\n");
			step = STEP_TURN;
			route = ROUTE_180;
			turn_0_or_180 = 1;
		}
		else if (step == STEP_FOUND_OBS && (obs_turn_cnt == 0 || obs_turn_cnt == 3)) {
			sn754410_turn_to(DEG_90);
			putrsUSART("turn_90\r\n");
			step = STEP_TURN;
			route = ROUTE_90;
			++obs_turn_cnt;
		}
		else if (step == STEP_FOUND_OBS && (obs_turn_cnt == 2  || obs_turn_cnt == 4)) {
			sn754410_turn_to(DEG_180);
			putrsUSART("turn_180\r\n");
			step = STEP_TURN;
			route = ROUTE_180;
			++obs_turn_cnt;
		}
		else if (step == STEP_FOUND_OBS && (obs_turn_cnt == 1 || obs_turn_cnt == 5)) {
			sn754410_turn_to(DEG_0);
			putrsUSART("turn_0\r\n");
			step = STEP_TURN;
			route = ROUTE_0;
			if (++obs_turn_cnt > 5) obs_turn_cnt = 0;
		}
		else if (step == STEP_FOUND_OBS_ON_W && obs_turn_cnt == 1) {
			sn754410_turn_to(DEG_180);
			putrsUSART("turn_180\r\n");
			step = STEP_TURN;
			route = ROUTE_180;
			obs_turn_cnt = 3;
			turn_0_or_180 = 1;
		}
		else if (step == STEP_FOUND_OBS_ON_W && obs_turn_cnt == 4) {
			sn754410_turn_to(DEG_0);
			putrsUSART("turn_0\r\n");
			step = STEP_TURN;
			route = ROUTE_0;
			obs_turn_cnt = 0;
			turn_0_or_180 = 0;
		}


		/*Start PWM*/
		if (DataRdyUSART()) {
			unsigned char res = ReadUSART();
			
			if (res == 0x41) {
				putrsUSART("0 Deg");
				sn754410_turn_to(DEG_0);
			}
			else if (res == 0x42) {
				putrsUSART("90 Deg");
				sn754410_turn_to(DEG_90);
			}
			else if (res == 0x43) {
				putrsUSART("180 Deg");
				sn754410_turn_to(DEG_180);
			}
			else if (res == 0x44) {
				putrsUSART("270 Deg");
				sn754410_turn_to(DEG_270);
			}
			else if (res == 0x45) {
				putrsUSART("Frw");
				sn754410_fwd();
			}
			else if (res == 0x46) {
				putrsUSART("STOP");
				sn754410_brk();
			}
			else if (res == 0x53) {
				putrsUSART("CALIB");
				sn754410_turn_to(DEG_0);
				step = STEP_CALIB;
				route = ROUTE_0;
			}
			else {
				putrsUSART("INVALID");
			}
		}
    }

	CloseEPWM1();
	CloseI2C();
    CloseUSART();
}

