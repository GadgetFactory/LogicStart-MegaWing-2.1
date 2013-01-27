#ifndef MULTIPLEX7SEGSW_H
#define MULTIPLEX7SEGSW_H

#include "WProgram.h"
#include <avr/interrupt.h>

#define DOT 0x80

#ifdef SEVENSEG_COMMON_CATHODE
#define SEG_OFF		LOW
#define SEG_ON		HIGH
#else
#define SEG_OFF		HIGH
#define SEG_ON		LOW
#endif


namespace Multiplex7SegSW 
{
	extern byte digits[8];
	extern byte noOfDigits;
	extern byte enableZB;
	extern byte *digitsPtr; 
	extern byte *digitPinsPtr;
	extern byte *segmentPinsPtr; 
	extern byte digitCurrent;
	extern unsigned long millisNext;
	void set(byte _enableZB, byte _noOfDigits, byte *digitPins, byte *segmentPins);
	void loadValue(int value);
	void writeChar(int pos, byte chr);
	void update();
}
#endif
