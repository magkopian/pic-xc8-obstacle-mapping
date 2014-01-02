/* 
 * File:   hmc5883l.h
 * Author: Manolis Agkopian
 *
 * Created on 12 Δεκέμβριος 2013, 6:23 μμ
 */

#ifndef HMC5883L_H
#define	HMC5883L_H

#ifdef	__cplusplus
extern "C" {
#endif

#define HMC5883L_RD 0x3D
#define HMC5883L_WR 0x3C

#define HMC5883L_CRA  0x00
#define HMC5883L_CRB  0x01
#define HMC5883L_MR   0x02

#define HMC5883L_DXRA 0x03
#define HMC5883L_DXRB 0x04
#define HMC5883L_DZRA 0x05
#define HMC5883L_DZRB 0x06
#define HMC5883L_DYRA 0x07
#define HMC5883L_DYRB 0x08

#define HMC5883L_SR   0x09
#define HMC5883L_IRA  0x0A
#define HMC5883L_IRB  0x0B
#define HMC5883L_IRC  0x0C

// Initializes hmc5883l module
void hmc5883l_init(void);

// Read x, y and z axis from hmc5883l module
void hmc5883l_read(int *x, int *y, int *z);

#ifdef	__cplusplus
}
#endif

#endif	/* HMC5883L_H */

