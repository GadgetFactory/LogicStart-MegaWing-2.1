#define SEVENSEG_COMMON_ANODE

#include <Multiplex7SegSW.h>

/*
  Gadget Factory
  LogicStart MegaWing 2.x
  BPM7003
 
 
 BPM7003 LogicStart MegaWing Reference:
 http://papilio.cc/index.php?n=Papilio.LogicStartMegaWing
 
 created 2013
 by Girish Pundlik
 http://www.gadgetfactory.net
 
 This example code is in the public domain.
 */

int ledPins[] = { 
  39, 38, 37, 36, 35, 34, 33, 32  };       // an array of pin numbers to which LEDs are attached
#define ledCount  8           // the number of pins (i.e. the length of the array)

int buttonPins[] = { 
  47, 46, 45, 44, 43, 42, 41, 40 };       // an array of pin numbers to which Buttons are attached
#define  buttonCount 8           // the number of pins (i.e. the length of the array)

#define MODE_RGB          0
#define MODE_ANIMATION    1
#define MODE_NAVIGATION   2

#define RGB_R   29
#define RGB_G   30
#define RGB_B   31

#define JOY_UP      47
#define JOY_DOWN    46
#define JOY_LEFT    43
#define JOY_RIGHT   44
#define JOY_SELECT  45

byte digitPins[] = {11, 8, 2, 0, 28}; // LSB to MSB
#define  noOfDigits  5
byte segmentPins[] = {7, 10, 5, 6, 3, 4, 9, 1}; // seg a to g and DP

// variables will change:
unsigned long nextframe;
int buttonState = 0;         // variable for reading the pushbutton status
int thisPin = 0;
int ledPin = 0;
int disp = 0;
int currentset = 0;
int frame = 0;
int appmode = 0;

typedef byte (oneFrame)[5];

oneFrame frameHello[2] = {{'H', 'E', 'L', 'O', ' '}, {' ', ' ', ' ', ' ', ' '}};
oneFrame frameLines[3] = {{'*', '*', '*', '*', ' '}, {'-', '-', '-', '-', ' '}, {'&', '&', '&', '&', ' '}};
oneFrame frameClock[2] = {{'1', '0', '2', '5', ':'}, {'1', '0', '2', '5', ' '}};
oneFrame frameTemp[2] = {{'2', '8'+DOT, '5', 'C', '\''}, {'8', '3'+DOT, '3', 'F', '\''}};
oneFrame frameSprites[2] = {{';', 'O', ';', 'O', ' '}, {'O', ';', 'O', ';', ' '}};
oneFrame frameBouncingBall[4] = {{';', ' ', ' ', ' ', ' '}, {' ', 'O', ' ', ' ', ' '}, {' ', ' ', ';', ' ', ' '}, {' ', ' ', ' ', 'O', ' '}};

oneFrame frmUp     = {' ', ' ', 'U', 'P', ' '};
oneFrame frmDown   = {'D', 'O', 'W', 'N', ' '};
oneFrame frmLeft   = {'L', 'E', 'F', 'T', ' '};
oneFrame frmRight  = {'R', 'I', 'T', 'E', ' '};
oneFrame frmSelect = {' ', 'S', 'E', 'L', ' '};

oneFrame* frameSets[6] = {frameHello, frameLines, frameSprites, frameClock, frameTemp, frameBouncingBall};
oneFrame* currentFrame;
int frameCounts[6] = {2, 3, 2, 2, 2, 4};
int frameDelays[6] = {500, 250, 400, 500, 1000, 400};
  
#define maxframes 1

void setup() {
  // initialize the serial port
  Serial.begin(9600);

  // initialize the7-segment multiplex library
  Multiplex7SegSW::set(1, noOfDigits, digitPins, segmentPins);

  // initialize the LED pins as an outputs
  for (thisPin = 0; thisPin < ledCount; thisPin++)  {
    pinMode(ledPins[thisPin], OUTPUT);
    digitalWrite(ledPins[thisPin], LOW);
  }
  
  // initialize the switch (and joystick) pins as an inputs
  for (thisPin = 0; thisPin < ledCount; thisPin++)  {
    pinMode(buttonPins[thisPin], INPUT);
  }  
    pinMode(RGB_R, OUTPUT);
    pinMode(RGB_G, OUTPUT);
    pinMode(RGB_B, OUTPUT);
    digitalWrite(RGB_R, HIGH);
    digitalWrite(RGB_G, HIGH);
    digitalWrite(RGB_B, HIGH);
    nextframe = millis();
    currentFrame = frameSets[currentset];
}

void loop(){

	// Monitor the switches
	// 3 Left most switches set the application mode
	appmode = 0;
	if (digitalRead(buttonPins[7]) == HIGH)
	{
	  appmode |= 0x4;
	  digitalWrite(ledPins[2], HIGH);
	}
	else
	  digitalWrite(ledPins[2], LOW);

	if (digitalRead(buttonPins[6]) == HIGH)
	{
	  appmode |= 0x2;
	  digitalWrite(ledPins[1], HIGH);
	}
	else
	  digitalWrite(ledPins[1], LOW);

	if (digitalRead(buttonPins[5]) == HIGH)
	{
	  appmode |= 0x1;
	  digitalWrite(ledPins[0], HIGH);
	}
	else
	  digitalWrite(ledPins[0], LOW);
	// We have the application mode: either RGB, Animation or Navigation
	
	/* RGB MODE */
	// Switches 2, 1 and 0 control R, G and B LEDs
	// Setting the button to LOW will switch on the respective LED.
	// 8 combinations can be displayed with the 3 LEDs.
    if (appmode == MODE_RGB)
    {
      if (digitalRead(buttonPins[2]) == HIGH)
          digitalWrite(RGB_R, HIGH);
         else
         digitalWrite(RGB_R, LOW);
      if (digitalRead(buttonPins[1]) == HIGH)
          digitalWrite(RGB_G, HIGH);
         else
         digitalWrite(RGB_G, LOW);
      if (digitalRead(buttonPins[0]) == HIGH)
          digitalWrite(RGB_B, HIGH);
         else
         digitalWrite(RGB_B, LOW);      
    }
    
	/* ANIMATION MODE */
	// Lowest 3 bits set the animation sequence
	// 000: Flashing 'Helo'
	// 001: Horizontal Line rolling vertically
	// 010: Clock with flashing colon (set to 10:25)
	// 011: Temparature display flipping between °C and °F (fake value, not read from any sensor)
	// 100: Bouncing sprites
	// 1001 Bouncing ball that goes from left to right
	//
	// All other values are ignored
    if (appmode == MODE_ANIMATION)
    {
      disp = 0;
      if (digitalRead(buttonPins[2]) == HIGH)
          disp |= 0x04;
      if (digitalRead(buttonPins[1]) == HIGH)
          disp |= 0x02;
      if (digitalRead(buttonPins[0]) == HIGH)
          disp |= 0x01;
          
     // If button 4 is low, go to animation mode
      // Counter displays a clock, temperature, hello, middle line, bouncing balls on joystick buttons
      
      if (currentset != disp)
      {
        if (disp <= 5)
        {
           currentset = disp;
           currentFrame = frameSets[currentset];
           frame = 0;
        }
      }

      // Write the frame to display
      if (millis() > nextframe)
      {
        for (int i = 0; i < noOfDigits; i++)
        {
          Multiplex7SegSW::writeChar(i, currentFrame[frame][i]);
        }
        frame++;
        if (frame >= frameCounts[currentset])
            frame = 0;
            
        nextframe = millis() + frameDelays[currentset];
      }
    }
	
	/* NAVIGATION MODE */
	// NOTE: Move switches 0 through 5 to top position in order
	// to allow the joystick to change signal levels
	// Operating the joystick will show its position on the
	// 7-segment display. All four directions and select button
	// can be detected
    if (appmode == MODE_NAVIGATION)
    {
      // Check if up
      if (digitalRead(JOY_UP) == LOW)
      {
              for (int i = 0; i < noOfDigits; i++)
        {
          Multiplex7SegSW::writeChar(i, frmUp[i]);
        }
      }
      // Check if down
      else if (digitalRead(JOY_DOWN) == LOW)
      {
              for (int i = 0; i < noOfDigits; i++)
        {
          Multiplex7SegSW::writeChar(i, frmDown[i]);
        }
      }
      // Check if left
      else if (digitalRead(JOY_LEFT) == LOW)
      {
              for (int i = 0; i < noOfDigits; i++)
        {
          Multiplex7SegSW::writeChar(i, frmLeft[i]);
        }
      }
      // Check if right
      else if (digitalRead(JOY_RIGHT) == LOW)
      {
              for (int i = 0; i < noOfDigits; i++)
        {
          Multiplex7SegSW::writeChar(i, frmRight[i]);
        }
      }
      // Check if select
      else if (digitalRead(JOY_SELECT) == LOW)
      {
              for (int i = 0; i < noOfDigits; i++)
        {
          Multiplex7SegSW::writeChar(i, frmSelect[i]);
        }
      }
      else
      {
                      for (int i = 0; i < noOfDigits; i++)
        {
          Multiplex7SegSW::writeChar(i, ' ');
        }
      }
      
    }

    Multiplex7SegSW::update();
}

