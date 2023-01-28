/**************************************************/
/*																					*/
/*  FILE        :Hwsetup.cpp												*/
/*  DATE        :Mon, Oct 13, 2014										*/
/*  DESCRIPTION :Hardware Setup program file.					*/
/*  CPU GROUP   :34C													*/
/*																					*/
/**************************************************/

#include "sfr_r834c.h"					/* Definition of the R8C/34C SFR */

static void mcu_init(void);
static void port_init(void);
static void motor_init(void);
static void ad_init(void);

//
void Hwsetup()
{
	mcu_init();
	port_init();
	ad_init();
	motor_init();
}
//Set High-speed on-chip oscillator clock to System clock
static void mcu_init()
{
	prc0 = 1;		/* Protect off *///prc0 prcr_addr.bit.b0  CM0、CM1、CM3、OCD、FRA0、FRA1、FRA2、FRA3
	cm14 = 0;		/* Low-speed on-chip oscillator off */
	fra2 = 0x00;	/* Selects Divide-by-2 mode */
	fra00 = 1;		/* High-speed on-chip oscillator on */
	for( int n = 0; n <= 255; n++);		/* This setting is an example of waiting time for the */
						/* oscillation stabilization. Please evaluate the time */
						/* for oscillation stabilization by a user. */
	fra01 = 1;		/* Selects High-speed on-chip oscillator */
	ocd2 = 1;		/* System clock : On-chip oscillator clock selected */
	cm16 = 0;		/* No division mode in system clock division */
	cm17 = 0;
	cm06 = 0;		/* CM16 and CM17 enable */
	prc0 = 0;		/* Protect on */
}
//
static void port_init()
{
	//LED port setting
	//P0_5 LEDA
	//P0_6 LEDB
	//P0_7 LEDC
	/* Setting port registers */
	p0 = 0;				/* LED port off */
	/* Setting port direction registers */
	prc2 = 1;			/* Protect off */
	pd0 |= 0xE0;		/* LED port direction = output */
	
	//SWITCH Button & Motor Output
	p2 = 0 ;				//P2_0=Switch Input, P2_1...P2_7=Motor Output
	pu04 = 1 ;			//Pullup for Switch Input
	pd2 = 0xFE ;		//P2_0 Input, P2_1...P2_7 Output
	
	//P6_6,P6_7
	pd6_6 = 1 ;	//TP7
	pd6_7 = 1 ;	//TP6
	drr15 = 1 ;	//Drive High
	p6_6 = 1 ;	//TP7
	p6_7 = 1 ;	//TP6
}
//
static void motor_init()
{
	// Move the motor in timer RD PWM mode.
	//Motor port setting 
	pd2 |= 0xFE ;			//P2_1,P2_3,P2_6,P2_7 output
	trdpsr0 = 0x08;		// Timer RD pin selection register0 TRDIOB0=>P2_2
	trdpsr1 = 0x01;		// Timer RD pin selection register1 TRDIOA1=>P2_4
	trdmr = 0x70;			// Timer RD mode register=>Buffer operation of register
	trdfcr = 0x01;			// Timer RD function control register=>Set to reset-synchronized PWM mode
	trdoer1 = 0xed;		// Timer RD output master enable register1
								// TRDIOB0 TRDIOA1 => Allow PWM output of pin
	trdcr0 = 0x23;			// Timer RD control register0 =>With compare match with TRDGRA0
								// TRD0Register Clear, Count on rising edge, f8
	trdsr0 = 0x00;			// Timer RD status register0
	trdsr1 = 0x00;			// Timer RD status register1

	trdier0 = 0x00;			// Timer RD interrupt enable register0 =>Do not allow interrupts
	trdier1 = 0x00;			// Timer RD interrupt enable register1 =>Do not allow interrupts
	trd0 = 0;					// Timer RD counter0

	trdgra0 = trdgrc0 = 0xFFFF;		// period
	trdgrb0 = trdgrd0 = 0xFFFF;		// PWM output1=TRDIOB0←Left Motor,TRDIOD0
	trdgra1 = trdgrc1 = 0xFFFF;		// PWM output2=TRDIOA1←Right Motor,TRDIOC1
}
/* A/D conversion initialize */
static void ad_init()
{
	//Use timer RA for AD conversion trigger, 10ms
	tracr = 0x00 ;		//Timer Stop
	tramr = 0x10 ;		//Timer mode, f/8
	trapre = 128 - 1 ;	//
	tra = 195 - 1 ;		//195=>10ms
	traic = 0x05 ;		//Enable RA interrupt level 5
	tracr = 0x01 ;		//Timer Start
	
	prc2 = 1;			/* Protect off */
	pd0 &= 0xF0;		/* AN4/P0_3..An7/P0_0 port direction = input */
	adic = 0x05;		/* Enable A-D conversion interrupt level 5 */
	adcon0 = 0x00;	/* Stop A/D conversion */
	prc3 = 1;			/* Protect off */
	ocvrefan = 0;		/* On-chip reference voltage and analog input are cut off */
	prc3 = 0;			/* Protect on */
	admod = 0x22;		/* Division select : fAD divided by 2 */
							/* Clock source : f1 */
							/* A/D operating mode : one-sweep mode */
							/* A/D conversion starts by software trigger */

	for( int n = 0; n < 10; n++);	/* When the CKS2 bit is changed, wait for 3 φAD cycles */
							/* or more before starting A/D conversion */

    adinsel = 0x30;	/* Analog input pin : AN4, AN5, AN6, AN7 */
                   			/* A/D sweep pin count : 8 pins */
                   			/* A/D input group : Port P0 */

	adcon1 = 0x30;	/* Extended analog input pin not selected */
							/* 10-bit mode */
							/* A/D operation enabled */
							/* A/D open-circuit detection assist function：disabled */

	for(int n = 0; n < 10; n++);	/* When the ADSTBY bit is changed */
							/* from 0 (A/D operation stops) to */
							/* 1 (A/D operation enabled), wait for 1 φAD cycle */
}
