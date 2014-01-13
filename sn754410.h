/* 
 * File:   sn754410.h
 * Author: Manolis Agkopian
 *
 * Created on 2 Ιανουάριος 2014, 8:32 μμ
 */

#ifndef SN754410_H
#define	SN754410_H

#ifdef	__cplusplus
extern "C" {
#endif

#define EN12 PORTBbits.RB0
#define EN34 PORTBbits.RB1

#define TRIS_EN12 TRISB0
#define TRIS_EN34 TRISB1

#define A4 PORTDbits.RD3
#define A2 PORTDbits.RD5
#define A3 PORTDbits.RD2
#define A1 PORTDbits.RD7
    
#define TRIS_A4 TRISD3
#define TRIS_A2 TRISD5
#define TRIS_A3 TRISD2
#define TRIS_A1 TRISD7

#define MOVE_STOP 0x00
#define MOVE_FORWARD 0x66
#define MOVE_REVERSE 0x99
#define MOVE_LEFT 0xF0
#define MOVE_RIGHT 0x0F

#define DEG_0 2
#define DEG_90 75
#define DEG_180 134
#define DEG_270 220

#define TOLERANCE 3
#define SSE 0

extern unsigned char last_action;
extern unsigned int turn_to;

// Initializes SN754410 H-Bridge
void sn754410_init();

// Make vehicle to move forward
void sn754410_fwd();

// Make vehicle to move backwards
void sn754410_rev();

// Make vehicle to turn left
void sn754410_trnl();

// Make vehicle to turn rigth
void sn754410_trnr();

// Make vehicle to break
void sn754410_brk();

// Make vehicle to turn a specific angle
int sn754410_turn_to(unsigned int trn_angle);

// Test turn to angle and correct or stop vehicle
void sn754410_test_turn_to(void);

// Stop the vehicle if brk_angle has been reached
int sn754410_break_if(double brk_angle);

#ifdef	__cplusplus
}
#endif

#endif	/* SN754410_H */

