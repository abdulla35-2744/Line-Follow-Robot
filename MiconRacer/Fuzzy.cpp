/**************************************************/
/*																					*/
/*  FILE        :Fuzzy.cpp													*/
/*  DATE        :Mon, Oct 13, 2014										*/
/*  DESCRIPTION :Fuzzy calculation program file.					*/
/*  CPU GROUP   :34C													*/
/*																					*/
/**************************************************/

#include "sfr_r834c.h"		/* Definition of the R8C/34C SFR */
#include "MiconRacer.h"

/*Global variables******************************/
//Make sure the left and right sensors ride on the black tape.
//Floating point can not be used, so multiply by 10 and calculate it as an integer.
//Difference between right and left sensor values
static const long deviation[] = { -1600, -800, 0, 800, 1600};
//Change in difference between left and right sensor values
static const long change[] = { -300, -150, 0, 150, 300};
//Output value
static const long output[] = { -200, -100, 0, 100, 200};
static long oldVal ; //Last difference
//Because we can not afford the stack, we made it globally.
static long data1[5];
static long data2[5];
static long result[5][5];
static long dA ;
static long dB ;
static long oNB ;
static long oNS ;
static long oZO ;
static long oPS ;
static long oPB ;
static const short areaPB[6][2] = {{0,0},{0,1},{0,2},{1,0},{1,1},{2,0}};
static const short areaPS[4][2] = {{0,3},{1,2},{2,1},{3,0}};
static const short areaZO[5][2] = {{0,4},{1,3},{2,2},{3,1},{4,0}};
static const short areaNS[4][2] = {{1,4},{2,3},{3,2},{4,1}};
static const short areaNB[6][2] = {{2,4},{3,3},{3,4},{4,2},{4,3},{4,4}};
static long gravity ;

//Initialization
void InitFuzzy()
{
    oldVal = 0 ;
}
//Fuzzy reasoning
int Fuzzy(int newVal)
{
	long now = newVal * 10;    //This time difference
	long diff = now - oldVal ; //Change in difference
	oldVal = now ;
	long nume = 0;
	long deno = 0;

	//Part 1
	int n ;
	for ( n = 0; n < 5 ; n++)
		data1[n] = 0;
	if ( now < deviation[0])
		data1[0] = 10;
	else if ( now >= deviation[4])
		data1[4] = 10;
	else
	{
		for ( n = 0; n < 4 ; n++)
		{
			if ( now >= deviation[n] && now < deviation[n+1])
			{
				nume = ( now - deviation[n] ) * 10;
				deno = deviation[n+1] - deviation[n];
				if ( deno > 0)
				{
					data1[n+1] = nume / deno;
					data1[n] = 10 - data1[n+1];
				}
				break;
			}
		}
	}
	
	//Part 2
	for ( n = 0; n < 5 ; n++)
		data2[n] = 0;
	if ( diff < change[0])
		data2[0] = 10;
	else if ( diff >= change[4])
		data2[4] = 10;
	else
	{
		for ( n = 0; n < 4 ; n++)
		{
			if ( diff >= change[n] && diff < change[n+1])
			{
				nume = ( diff - change[n] ) * 10;
				deno = change[n+1] - change[n];
				if ( deno > 0)
				{
					data2[n+1] = nume / deno;
					data2[n] = 10 - data2[n+1];
				}
				break;
			}
		}
	}
	//AND operation
	dA = 0;
	dB = 0;
	int m ;
	for ( m = 0 ; m < 5 ; m++)
	{
		for ( n = 0 ; n < 5 ; n++)
		{
			result[m][n] = 0;
		}
	}
	for ( m = 0 ; m < 5 ; m++)
	{
		dA = data1[m];
		for ( n = 0; n < 5 ; n++)
		{
			dB = data2[n];
			if ( dA <= dB)
				result[m][n] = dA;
			else
				result[m][n] = dB;
		}
	}
	//OR operation
	oNB = 0;
	oNS = 0;
	oZO = 0;
	oPS = 0;
	oPB = 0;
	int j, k;
	//Max of oPB
	for ( n = 0; n < 6 ; n++)
	{
		j = areaPB[n][0];
		k = areaPB[n][1];
		if ( n == 0)
			oPB = result[j][k];
		else
		{
			if ( oPB < result[j][k])
				oPB = result[j][k];
		}
	}
	//Max of oPS
	for ( n = 0; n < 4 ; n++)
	{
		j = areaPS[n][0];
		k = areaPS[n][1];
		if ( n == 0)
			oPS = result[j][k];
		else
		{
			if ( oPS < result[j][k])
				oPS = result[j][k];
		}
	}
	//Max of oZO
	for ( n = 0; n < 5 ; n++)
	{
		j = areaZO[n][0];
		k = areaZO[n][1];
		if ( n == 0)
			oZO = result[j][k];
		else
		{
			if ( oZO < result[j][k])
				oZO = result[j][k];
		}
	}
	//Max of oNS
	for ( n = 0; n < 4 ; n++)
	{
		j = areaNS[n][0];
		k = areaNS[n][1];
		if ( n == 0)
			oNS = result[j][k];
		else
		{
			if ( oNS < result[j][k])
				oNS = result[j][k];
		}
	}
	//Max of oNB
	for ( n = 0; n < 6 ; n++)
	{
		j = areaNB[n][0];
		k = areaNB[n][1];
		if ( n == 0)
			oNB = result[j][k];
		else
		{
			if ( oNB < result[j][k])
				oNB = result[j][k];
		}
	}
	//Gravity
	gravity = 0;
	deno = oNB + oNS + oZO + oPS + oPB ;
	if ( deno > 0)
	{
		gravity = oNB * output[0] + oNS * output[1] + oZO * output[2] + oPS * output[3] + oPB * output[4];
		gravity /= deno;
	}
	return (int)( gravity / 10 );
}
