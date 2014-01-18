/* 
 * File:   main.c
 * Author: Manolis Agkopian
 *
 * Created on 18 Δεκέμβριος 2013, 3:42 μμ
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

extern double angle;

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
			TMR3L = 0;
			TMR3H = 0;
			PIR2bits.TMR3IF = 0;
			T3CONbits.TMR3ON = 1;
		}


		//int x = 0, y = 0, z = 0;
		//char buff[20];

		// Get x and y from HMC5883L
		//hmc5883l_read(&x, &y, &z);

		// Convert to degrees
		//angle = atan2((double) y, (double) x) * (180.0 / 3.14159265) + 180.0;



        //sprintf(buff, "%f\r\n", angle);
        //putsUSART(buff);

		//sprintf(buff, "%d\r\n", us020_read());
        //putsUSART(buff);

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
			else {
				putrsUSART("INVALID");
			}
		}
    }

	CloseEPWM1();
	CloseI2C();
    CloseUSART();
}

