/**************************************************/
/*																					*/
/*  FILE          :Analog.cpp												*/
/*  DATE        :Mon, Oct 13, 2014										*/
/*  DESCRIPTION :Analog Input program file.						*/
/*  CPU GROUP   :34C													*/
/*																					*/
/**************************************************/

#include "sfr_r834c.h"					/* Definition of the R8C/34C SFR */
#include "MiconRacer.h"

//A/D conversion
#define	AIKAZ	4
static WORD adc_data[AIKAZ] ;			//Raw data for input check

//
void InitAdc()
{
	for ( int n = 0 ; n < AIKAZ ; n++ ){
		adc_data[n] = 0 ;
	}
}
//Use timer RA for AD conversion trigger (Used in pulse output mode for confirmation) 10ms
#pragma interrupt intTRAIC(vect=22)
void intTRAIC(void)
{
	adst = 1 ;		//ADC Start
}
//
//The input value is small when it is bright, and the value is large when it is dark.
#pragma interrupt/B intADIC(vect=14)
void intADIC(void)
{
	/* ---- Write A/D conversion result ---- */
	adc_data[0] = ad4 & 0x03FF ;	/* Write conversion result of AN4 to buffer */
	adc_data[1] = ad5 & 0x03FF ;	/* Write conversion result of AN5 to buffer */
	adc_data[2] = ad6 & 0x03FF ;	/* Write conversion result of AN6 to buffer */
	adc_data[3] = ad7 & 0x03FF ;	/* Write conversion result of AN7 to buffer */
}
//
int GetValue(int n)
{
	return adc_data[n] ;
}
//
void GetValues(int data[])
{
	for ( int n = 0 ; n < 4 ; n++ )
	{
		data[n] = adc_data[n] ;
	}
}