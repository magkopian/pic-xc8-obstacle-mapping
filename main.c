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

#define _XTAL_FREQ 12000000

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

	for (;;) {
		/*Calculate Angle*/
        char buff[20];
        int x = 0, y = 0, z = 0;
        double angle;

        // Get x and y from HMC5883L
        hmc5883l_read(&x, &y, &z);

		// Convert to degrees
        angle = atan2((double) y, (double) x) * (180.0 / 3.14159265) + 180.0;

        sprintf(buff, "%f\r\n", angle);
        putsUSART(buff);

		/*Start PWM*/
		if (DataRdyUSART()) {
			unsigned char res = ReadUSART();
			
			if (res == 0x41) {
				putrsUSART("MOVE_FORWARD");
				sn754410_fwd();
			}
			else if (res == 0x42) {
				putrsUSART("MOVE_REVERSE");
				sn754410_rev();
			}
			else if (res == 0x43) {
				putrsUSART("TURN_LEFT");
				sn754410_trnl();
			}
			else if (res == 0x44) {
				putrsUSART("TURN_RIGHT");
				sn754410_trnr();
			}
			else if (res == 0x45) {
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
