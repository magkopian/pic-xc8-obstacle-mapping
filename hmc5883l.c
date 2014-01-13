/*
 * File:   hmc5883l.c
 * Author: Manolis Agkopian
 *
 * Created on 12 Δεκέμβριος 2013, 6:28 μμ
 */

#include <xc.h>
#include <plib/i2c.h>
#include <stdio.h>
#include <stdlib.h>
#include "hmc5883l.h"

#define _XTAL_FREQ 12000000

// Initializes hmc5883l module
void hmc5883l_init(void) {
	CloseI2C(); // Close I2C if was operating earlier

	OpenI2C(MASTER, SLEW_OFF); // Master Mode

	// Clock = FOSC/(4 * (SSPADD + 1))
	SSPADD = 29; // 100KHz BaudRate @ 12MHz FOSC

	StartI2C();

	WriteI2C(HMC5883L_WR); // Write Mode

	WriteI2C(HMC5883L_CRA); // Set register pointer to Config Reg A
	WriteI2C(0x78); // 8-average, 75 Hz, normal measurement
	WriteI2C(0xE0); // Gain=5
	WriteI2C(0x00); // Continuous-measurement mode

	StopI2C();
	
	__delay_ms(6);
}

// Read x, y and z axis from hmc5883l module
void hmc5883l_read(int *x, int *y, int *z) {
	unsigned char stat;

    /*Wait until status RDY bit is 18
    do {
      StartI2C();

      WriteI2C(HMC5883L_WR); // Write Mode
      WriteI2C(HMC5883L_SR); // Set register pointer to status register
      StopI2C();

      StartI2C();
      WriteI2C(HMC5883L_RD); // Read Mode
      stat = ReadI2C(); // Read status register

      StopI2C();
    } while (stat & 0x01 == 0x00);*/

	//__delay_ms(6);

    /*Read x, y and z axis*/
    StartI2C();

    WriteI2C(HMC5883L_WR); // Write Mode
    WriteI2C(HMC5883L_DXRA); // Set register pointer to data x register MSB

	RestartI2C();

    WriteI2C(HMC5883L_RD); // Read Mode

    /*Read x axis*/
    *x = (int) ReadI2C(); AckI2C(); // Read x axis MSB (ACK)
    *x <<= 8;
    *x |= (int) ReadI2C(); AckI2C(); // Read x axis LSB (ACK)

    /*Read z axis*/
    *z = (int) ReadI2C(); AckI2C(); // Read z axis MSB (ACK)
    *z <<= 8;
    *z |= (int) ReadI2C(); AckI2C(); // Read z axis LSB (ACK)

    /*Read y axis*/
    *y = (int) ReadI2C(); AckI2C(); // Read y axis MSB (ACK)
    *y <<= 8;
    *y |= (int) ReadI2C(); NotAckI2C(); // Read y axis LSB (NACK)
	
    StopI2C();
}
