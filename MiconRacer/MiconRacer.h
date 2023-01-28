typedef unsigned char BYTE ;
typedef unsigned short WORD ;

#include "Prototype.h"

//For digital input / output state, timer state
enum { OFF,ON,UP };
//Input logical table
enum { NEG, POS };


//Digital input port
enum {
	START_PB,		// 0
	INNUM
};

#define DARK		570		//03C4 => 964, 700->570
#define BRIGHT	450		//0038 => 56,  300->450
