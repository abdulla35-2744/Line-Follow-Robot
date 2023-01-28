/**************************************************/
/*																					*/
/*  FILE        :Motor.cpp 													*/
/*  DATE        :Mon, Oct 13, 2014										*/
/*  DESCRIPTION :Motor drive program file.							*/
/*  CPU GROUP   :34C													*/
/*																					*/
/**************************************************/

#include "sfr_r834c.h"					/* Definition of the R8C/34C SFR */
#include "MiconRacer.h"

//#define BLUELED		1	//When using BLUE LED
#define ALPHA	10		//

#define slowVal	60			//%
#define normalVal	70		//%
#define highVal	80			//%
#define PERIOD		40000
#define ZONE			20
#define abs(n) ((n >= 0) ? n : -n )
enum { STOP,FORWARD,BACKWARD };
#define CONFIRM2 2
#define CONFIRM4 4
#define CONFIRM6 16
static short counter[4] ;
enum { S_FOR, S_BACK, S_RET };
static short stage ;	//Control stage
static int oldResult ;	//Last result
long maxValue ;

static void Motor( int dir, int left, int right );
static void RightTurn( int left, int right );
static void LeftTurn( int left, int right );

//
void InitMotor( int rate )
{
	stage = S_FOR ;	//Control stage
	for ( int n = 0 ; n < 4 ; n++ )
		counter[n] = 0 ;
	oldResult = 0 ;		//Last result
	maxValue = ((long)PERIOD * rate ) /100 ;
	trdgra0 = trdgrc0 = PERIOD - 1;			// period
	trdgrb0 = trdgrd0 = maxValue / 2;		// PWM output1, TRDIOB0Å©Left Motor,TRDIOD0
	trdgra1 = trdgrc1 = maxValue / 2;		// PWM output2, TRDIOA1Å©Right Motor,TRDIOC1
	
	trdstr = 0x05;		// Timer RD start register. Start counting of TRD0
}
//
void MotorStart()
{
	stage = 0 ;	//Control stage
	for ( int n = 0 ; n < 4 ; n++ )
		counter[n] = 0 ;
	oldResult = 0 ;	//Last result
	Motor( FORWARD, normalVal, normalVal );
}
//
void MotorStop()
{
	Motor( STOP, 0, 0 );
}
//
void MotorControl()
{
	int diff = 0 ;
	int result = 0;
	int sensor[4];
	GetValues(sensor);
	switch ( stage ){
	case S_FOR :	//FORWARD
			counter[0] = counter[1] = 0;
			//When both sides leave the course
			if ( sensor[1] < BRIGHT && sensor[2] < BRIGHT )
			{
				if ( ++counter[2] >= CONFIRM2 )	//For confirmation
				{
					counter[2] = 0 ;
					Motor( STOP, 0, 0 );	//Stop it once
					oldResult = 0 ;
					stage = S_BACK ;			//fall back
#ifdef BLUELED
					p6_6 = 0 ;	//TP7
					p6_7 = 0 ;	//TP6
#endif
				}
			}
			//When on the course
			else
			{
				counter[2] = 0;
				diff = sensor[1] - sensor[2] ;
				result = Fuzzy( diff );
				if ( oldResult != result )
				{
					oldResult = result ;	//Last result
					//When the difference between the left and right is small
					if ( abs(result) < ZONE )
					{
						int base = normalVal + ALPHA ;
						Motor( FORWARD, base, base );	//Continue to move forward
					}
					//When the difference between left and right is large
					else
					{
						int base = slowVal;	//Reduce the speed
						Motor( FORWARD, base+result, base-result );	//To change the direction
#ifdef BLUELED
						if ( result > 0 )
						{
							p6_6 = 0 ;	//TP7
							p6_7 = 1 ;	//TP6
						}
						else
						{
							p6_6 = 1 ;	//TP7
							p6_7 = 0 ;	//TP6
						}
#endif
					}
				}
			}
		break ;
	case S_RET :	//Return to progress
		counter[0] = counter[1] = counter[2] = 0 ;
		oldResult = 9999 ;
#ifdef BLUELED
		p6_6 = 1 ;	//TP7
		p6_7 = 1 ;	//TP6
#endif
		stage = S_FOR ;
		break ;
	default :
		stage = S_FOR ;
	}
}

//If the voltage of the battery is insufficient, only one of the motors may move.
static void Motor( int dir, int left, int right )
{
	if ( dir == STOP )	//Stop
	{
		p2_1 = 0 ;
		p2_6 = 0 ;
		p2_3 = 0 ;
		p2_7 = 0 ;
	}
	else	//Forward and backward
	{
		trdgrd0 = (maxValue * left) / 100 ;	// PWM output1, TRDIOB0Å©Left motor,TRDIOD0
		trdgrc1 = (maxValue * right) / 100;	// PWM output2, TRDIOA1Å©Right motor,TRDIOC1
		if ( dir == FORWARD )
		{
			p2_1 = 0 ;	//Left wheel
			p2_6 = 1 ;
			p2_3 = 0 ;	//Right wheel
			p2_7 = 1 ;
		}
		else
		{
			p2_1 = 1 ;	//Left wheel
			p2_6 = 0 ;
			p2_3 = 1 ;	//Right wheel
			p2_7 = 0 ;
		}
	}
}
//Right turn, Left wheelÅÅFORWARD, Right wheelÅÅBACKWARD
static void RightTurn( int left, int right )
{
	trdgrd0 = (maxValue * left) / 100 ;	// PWM output1, TRDIOB0Å©Left motor,TRDIOD0
	trdgrc1 = (maxValue * right) / 100;	// PWM output2, TRDIOA1Å©Right motor,TRDIOC1
	p2_1 = 0 ;	//Left wheelÅÅFORWARD
	p2_6 = 1 ;
	p2_3 = 1 ;	//Right wheelÅÅBACKWARD
	p2_7 = 0 ;
}
//Left turn, Left wheelÅÅBACKWARD, Right wheelÅÅFORWARD
static void LeftTurn( int left, int right )
{
	trdgrd0 = (maxValue * left) / 100 ;	// PWM output1, TRDIOB0Å©Left motor,TRDIOD0
	trdgrc1 = (maxValue * right) / 100;	// PWM output2, TRDIOA1Å©Right motor,TRDIOC1
	p2_1 = 1 ;	//Left wheelÅÅBACKWARD
	p2_6 = 0 ;
	p2_3 = 0 ;	//Right wheelÅÅFORWARD
	p2_7 = 1 ;
}
