/**************************************************/
/*																					*/
/*  FILE        :Input.cpp													*/
/*  DATE        :Mon, Oct 13, 2014										*/
/*  DESCRIPTION :Digital Input program file.						*/
/*  CPU GROUP   :34C													*/
/*																					*/
/**************************************************/

#include "sfr_r834c.h"					/* Definition of the R8C/34C SFR */
#include "MiconRacer.h"

//Input chattering cancel table
struct inCtr {
	unsigned short on ;
	unsigned short off ;
};
static struct inCtr inTbl[INNUM] ;	//Digital input table
	//Input definition table
const bool logic[INNUM] = {
	  NEG,	// 0 START_PB
};
static const unsigned short LIMIT = 50 ;
static const unsigned short MANY = 45 ;
static int step ;
//
void InitInput()
{
	struct inCtr *sp = inTbl ;
	for ( int n = INNUM ; n-- > 0 ; ) {
		sp->on = 0 ;
		sp->off = 0 ;
		sp++ ;
	}
	step = 0 ;
}
//Digital input
void Input()
{
	int n ;
	struct inCtr *sp = inTbl ;
	for ( int m = 0 ; m < INNUM ; m++ ){
		switch ( m ){
		case START_PB :
			n = p2_0 ;
			break ;
		default :
			n = 0 ;
		}
		if ( n ){
			if ( sp->on < LIMIT )
				sp->on++ ;
			if ( sp->off > 0 )
				sp->off-- ;
		}
		else {
			if ( sp->off < LIMIT )
				sp->off++ ;
			if ( sp->on > 0 )
				sp->on-- ;
		}
		sp++ ;
	}
}
//
int ChkIn( int n )
{
	struct inCtr *sp ;
	const bool *sq ;
	
	sp = &inTbl[n] ;
	sq = &logic[n] ;
	
	if ( sp->on > MANY )
		return ( *sq == POS ) ? ON : OFF ;
	else if ( sp->off > MANY )
		return ( *sq == POS ) ? OFF : ON ;
	else
		return -1 ;
}
//Has the start button been pressed?
bool IsStartPB()
{
	bool flag = false ;
	
	switch( step )
	{
		case 0 :
			if ( ChkIn(START_PB) == ON )
			{
				flag = true ;
				step++ ;
			}
			break;
		case 1 :
			if ( ChkIn(START_PB) == OFF )
				step = 0 ;
			break;
		default :
			step = 0;
	}
	return flag ;
}