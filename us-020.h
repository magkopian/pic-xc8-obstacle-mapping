/* 
 * File:   us-020.h
 * Author: Manolis Agkopian
 *
 * Created on 17 Ιανουάριος 2014, 1:30 πμ
 */

#ifndef US_020_H
#define	US_020_H

#ifdef	__cplusplus
extern "C" {
#endif

#define TRIGGER PORTBbits.RB2
#define TRIS_TRIGGER TRISB2
#define ECHO PORTCbits.RC2
#define TRIS_ECHO TRISC2

void us020_init (void);
int us020_read (void);
    
#ifdef	__cplusplus
}
#endif

#endif	/* US_020_H */
