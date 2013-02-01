/*
	Multiplex7SegSw is a library that allows you to multiplex upto 5
	7 segment digits using any one of the pins on your arduino.
	
	This library is derived from the original hardware timer based
	Multiplex7SegSW library provide with the Arduino IDE, though it
	removes dependence on hardware timer and provides a pure Arduino
	code to operate a 5-digit 7-segment display. No periodic function
	is called automatically, so the main application must call the
	Multiplex7SegSW::update() function in the loop()
	
	This library also introduces a subset of ASCII character set
	so that the user can pass ASCII strings to the library. It is
	of course, not possible to display every character on a standard
	7-segment character block, so some special symbols are provided in
	these places. 
	
	Currently ASCII characters 0x20 through 0x5A (space through capital-Z)
	are supported, while some character codes in the range are used for
	special symbols that can be used to create certain combinations on the
	7-segment display.
	Please refer to the document 7segcharset.xls for details.
	
	This library was intended for the LogicStart MegaWing 2.0 demo.

	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2.1 of the License, or (at your option) any later version.

	This library is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public
	License along with this library; if not, write to the Free Software
	Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/
#include "WProgram.h"
#include "Multiplex7SegSW.h"

byte Multiplex7SegSW::digits[8];
byte Multiplex7SegSW::noOfDigits;
byte Multiplex7SegSW::enableZB;
byte *Multiplex7SegSW::digitsPtr; 
byte *Multiplex7SegSW::digitPinsPtr;
unsigned long Multiplex7SegSW::millisNext;
byte *Multiplex7SegSW::segmentPinsPtr; 
byte Multiplex7SegSW::digitCurrent;

byte SegmentData[72][8] = {
 //  a        b        c        d        e        f        g        dp
 {SEG_OFF, SEG_OFF, SEG_OFF, SEG_OFF, SEG_OFF, SEG_OFF, SEG_OFF, SEG_OFF}, // Blank
 {SEG_OFF, SEG_ON , SEG_OFF, SEG_ON , SEG_OFF, SEG_OFF, SEG_OFF, SEG_OFF}, // !
 {SEG_OFF, SEG_ON , SEG_OFF, SEG_OFF, SEG_OFF, SEG_ON , SEG_OFF, SEG_OFF}, // "
 {SEG_OFF, SEG_ON , SEG_ON , SEG_ON , SEG_ON , SEG_ON , SEG_ON , SEG_OFF}, // #
 {SEG_ON , SEG_OFF, SEG_OFF, SEG_ON , SEG_OFF, SEG_OFF, SEG_ON , SEG_OFF}, // $
 {SEG_OFF, SEG_OFF, SEG_ON , SEG_OFF, SEG_OFF, SEG_ON , SEG_ON , SEG_OFF}, // %
 
 {SEG_OFF, SEG_OFF, SEG_OFF, SEG_ON , SEG_OFF, SEG_OFF, SEG_OFF, SEG_OFF}, // Underline // &
 
 {SEG_OFF, SEG_OFF, SEG_ON , SEG_OFF, SEG_OFF, SEG_OFF, SEG_OFF, SEG_OFF}, // '
 {SEG_OFF, SEG_OFF, SEG_OFF, SEG_OFF, SEG_ON , SEG_ON , SEG_OFF, SEG_OFF}, // (
 {SEG_OFF, SEG_ON , SEG_ON , SEG_OFF, SEG_OFF, SEG_OFF, SEG_OFF, SEG_OFF}, // )
 
 {SEG_ON , SEG_OFF, SEG_OFF, SEG_OFF, SEG_OFF, SEG_OFF, SEG_OFF, SEG_OFF}, // Upperline *
 
 {SEG_OFF, SEG_ON , SEG_ON , SEG_OFF, SEG_OFF, SEG_OFF, SEG_ON , SEG_OFF}, // +
 {SEG_OFF, SEG_OFF, SEG_ON , SEG_OFF, SEG_OFF, SEG_OFF, SEG_OFF, SEG_OFF}, // ,
 {SEG_OFF, SEG_OFF, SEG_OFF, SEG_OFF, SEG_OFF, SEG_OFF, SEG_ON , SEG_OFF}, // -
 {SEG_OFF, SEG_OFF, SEG_OFF, SEG_ON , SEG_OFF, SEG_OFF, SEG_OFF, SEG_OFF}, // .
 {SEG_OFF, SEG_ON , SEG_OFF, SEG_OFF, SEG_ON , SEG_OFF, SEG_ON , SEG_OFF}, // /
 {SEG_ON , SEG_ON , SEG_ON , SEG_ON , SEG_ON , SEG_ON , SEG_OFF, SEG_OFF}, // 0
 {SEG_OFF, SEG_ON , SEG_ON , SEG_OFF, SEG_OFF, SEG_OFF, SEG_OFF, SEG_OFF}, // 1
 {SEG_ON , SEG_ON , SEG_OFF, SEG_ON,  SEG_ON , SEG_OFF, SEG_ON , SEG_OFF}, // 2
 {SEG_ON , SEG_ON , SEG_ON , SEG_ON , SEG_OFF, SEG_OFF, SEG_ON , SEG_OFF}, // 3
 {SEG_OFF, SEG_ON , SEG_ON , SEG_OFF, SEG_OFF, SEG_ON , SEG_ON , SEG_OFF}, // 4
 {SEG_ON , SEG_OFF, SEG_ON , SEG_ON , SEG_OFF, SEG_ON , SEG_ON , SEG_OFF}, // 5
 {SEG_ON , SEG_OFF, SEG_ON , SEG_ON , SEG_ON , SEG_ON , SEG_ON , SEG_OFF}, // 6
 {SEG_ON , SEG_ON , SEG_ON , SEG_OFF, SEG_OFF, SEG_OFF, SEG_OFF, SEG_OFF}, // 7
 {SEG_ON , SEG_ON , SEG_ON , SEG_ON , SEG_ON , SEG_ON , SEG_ON , SEG_OFF}, // 8
 {SEG_ON , SEG_ON , SEG_ON , SEG_OFF, SEG_OFF, SEG_ON , SEG_ON , SEG_OFF}, // 9
 
 {SEG_ON , SEG_ON , SEG_OFF, SEG_OFF, SEG_OFF, SEG_OFF, SEG_OFF, SEG_OFF}, // :
 {SEG_ON , SEG_ON , SEG_OFF, SEG_OFF, SEG_OFF, SEG_ON , SEG_ON , SEG_OFF}, // UpperO // ;
 
 {SEG_OFF, SEG_OFF, SEG_OFF, SEG_ON , SEG_ON , SEG_OFF, SEG_ON , SEG_OFF}, // <
 {SEG_OFF, SEG_OFF, SEG_OFF, SEG_ON , SEG_OFF, SEG_OFF, SEG_ON , SEG_OFF}, // =
 {SEG_OFF, SEG_OFF, SEG_ON , SEG_ON , SEG_OFF, SEG_OFF, SEG_ON , SEG_OFF}, // >
 {SEG_ON , SEG_ON , SEG_OFF, SEG_OFF, SEG_ON , SEG_OFF, SEG_ON , SEG_OFF}, // ?
 {SEG_ON , SEG_ON , SEG_OFF, SEG_ON , SEG_ON , SEG_ON , SEG_ON , SEG_OFF}, // @
 {SEG_ON , SEG_ON , SEG_ON , SEG_OFF, SEG_ON , SEG_ON , SEG_ON , SEG_OFF}, // A
 {SEG_OFF, SEG_OFF, SEG_ON , SEG_ON , SEG_ON , SEG_ON , SEG_ON , SEG_OFF}, // B
 {SEG_ON , SEG_OFF, SEG_OFF, SEG_ON , SEG_ON , SEG_ON , SEG_OFF, SEG_OFF}, // C
 {SEG_OFF, SEG_ON , SEG_ON , SEG_ON , SEG_ON , SEG_OFF, SEG_ON , SEG_OFF}, // D
 {SEG_ON , SEG_OFF, SEG_OFF, SEG_ON , SEG_ON , SEG_ON , SEG_ON , SEG_OFF}, // E
 {SEG_ON , SEG_OFF, SEG_OFF, SEG_OFF, SEG_ON , SEG_ON , SEG_ON , SEG_OFF}, // F
 {SEG_ON , SEG_ON , SEG_ON , SEG_ON , SEG_OFF, SEG_ON , SEG_ON , SEG_OFF}, // G
 {SEG_OFF, SEG_OFF, SEG_ON , SEG_OFF, SEG_ON , SEG_ON , SEG_ON , SEG_OFF}, // H
 {SEG_OFF, SEG_OFF, SEG_ON , SEG_OFF, SEG_OFF, SEG_OFF, SEG_OFF, SEG_OFF}, // I
 {SEG_OFF, SEG_ON , SEG_ON , SEG_ON , SEG_OFF, SEG_OFF, SEG_OFF, SEG_OFF}, // J
 {SEG_OFF, SEG_OFF, SEG_OFF, SEG_OFF, SEG_ON , SEG_ON , SEG_ON , SEG_OFF}, // K
 {SEG_OFF, SEG_OFF, SEG_OFF, SEG_ON , SEG_ON , SEG_ON , SEG_OFF, SEG_OFF}, // L
 {SEG_ON , SEG_OFF, SEG_ON , SEG_OFF, SEG_ON , SEG_OFF, SEG_ON , SEG_OFF}, // M
 {SEG_OFF, SEG_OFF, SEG_ON , SEG_OFF, SEG_ON , SEG_OFF, SEG_ON , SEG_OFF}, // N
 {SEG_OFF, SEG_OFF, SEG_ON , SEG_ON , SEG_ON , SEG_OFF, SEG_ON , SEG_OFF}, // O
 {SEG_ON , SEG_ON , SEG_OFF, SEG_OFF, SEG_ON , SEG_ON , SEG_ON , SEG_OFF}, // P
 {SEG_ON , SEG_ON , SEG_OFF, SEG_ON , SEG_OFF, SEG_ON , SEG_ON , SEG_OFF}, // Q
 {SEG_OFF, SEG_OFF, SEG_OFF, SEG_OFF, SEG_ON , SEG_OFF, SEG_ON , SEG_OFF}, // R
 {SEG_ON , SEG_OFF, SEG_ON , SEG_ON , SEG_OFF, SEG_ON , SEG_ON , SEG_OFF}, // S
 {SEG_OFF, SEG_OFF, SEG_OFF, SEG_ON , SEG_ON , SEG_ON , SEG_ON , SEG_OFF}, // T
 {SEG_OFF, SEG_ON , SEG_ON , SEG_ON , SEG_ON , SEG_ON , SEG_OFF, SEG_OFF}, // U
 {SEG_OFF, SEG_OFF, SEG_ON , SEG_ON , SEG_ON , SEG_OFF, SEG_OFF, SEG_OFF}, // V
 {SEG_OFF, SEG_ON , SEG_OFF, SEG_ON , SEG_OFF, SEG_ON , SEG_ON , SEG_OFF}, // W
 {SEG_ON , SEG_OFF, SEG_OFF, SEG_ON , SEG_OFF, SEG_OFF, SEG_ON , SEG_OFF}, // X
 {SEG_OFF, SEG_ON , SEG_ON , SEG_ON , SEG_OFF, SEG_ON , SEG_ON , SEG_OFF}, // Y
 {SEG_ON , SEG_OFF, SEG_OFF, SEG_ON , SEG_OFF, SEG_OFF, SEG_ON , SEG_OFF}, // Z
};

void Multiplex7SegSW::set(byte _enableZB, byte _noOfDigits, byte *digitPins, byte *segmentPins) // digitPins - LSB to MSB, segmentPins - a to g
{
	byte i;
  for (i = 0; i < _noOfDigits; i++) // initialize digit pins as OUTPUTS
  {
    pinMode(digitPins[i], OUTPUT);
	digitalWrite(digitPins[i], HIGH);
  }
  for (i = 0; i < 8; i++) // initialize segment pins as OUTPUTS
    pinMode(segmentPins[i], OUTPUT); 

	enableZB = _enableZB; // set zero leading blanking - 0 == OFF,  1 == ON
	noOfDigits = _noOfDigits; 
	digitsPtr = digits; // initialize digits pointer
  digitPinsPtr = digitPins; // initialize digit pins pointer
  segmentPinsPtr = segmentPins; // initialize segment pins pointer
	millisNext = millis();
	digitCurrent = 0;
}

void Multiplex7SegSW::loadValue(int value) // extract the integer value into decimal values
{
  *(digitsPtr + 3) = value / 1000;
  value %= 1000;
  *(digitsPtr + 2) = value / 100;
  value %= 100;
  *(digitsPtr + 1) = value / 10;
  *digitsPtr = value % 10;
}

void Multiplex7SegSW::writeChar(int pos, byte chr)
{
	digitsPtr[pos] = chr - 0x20;  // ASCII codes above 0x20 (space)
}

void Multiplex7SegSW::update() // interrupt activates update()
{
	if (millisNext < millis())
	{
		// Disable the display
		digitalWrite(*(digitPinsPtr + digitCurrent), HIGH);
	 
		// Increment index for next refresh
		digitCurrent++;
		if (digitCurrent == noOfDigits)
			digitCurrent = 0;
	 
		  byte chr = *(digitsPtr + digitCurrent);
		  byte *pat = SegmentData[chr & 0x7F];
		  digitalWrite(*(segmentPinsPtr), pat[0]);
		  digitalWrite(*(segmentPinsPtr + 1), pat[1]);
		  digitalWrite(*(segmentPinsPtr + 2), pat[2]);
		  digitalWrite(*(segmentPinsPtr + 3), pat[3]);
		  digitalWrite(*(segmentPinsPtr + 4), pat[4]);
		  digitalWrite(*(segmentPinsPtr + 5), pat[5]);
		  digitalWrite(*(segmentPinsPtr + 6), pat[6]);
		  if (chr & 0x80)
			digitalWrite(*(segmentPinsPtr + 7), SEG_ON);
		  else
			digitalWrite(*(segmentPinsPtr + 7), SEG_OFF);

		// Enable the display
		digitalWrite(*(digitPinsPtr + digitCurrent), LOW);
		
		millisNext = millis() + 3;
	}
}
