#include "ArduinoGraphics.h"
#include "Arduino_LED_Matrix.h"



namespace Matrix
{



ArduinoLEDMatrix matrix;



bool scroll = false;
int t = 0;

enum TEMP_STATUS
{
	ok = 0,
	cold = 1,
	hot = 2
};
TEMP_STATUS temp_status = ok;



void setup()
{
	matrix.begin();
	Serial.println("LED Matrix started.");
}



#define SCROLL_RATE 10		// period in seconds

void scrollingText()
{
	// LED Matrix Scrolling Text
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// !!! Will hang program (unresponsive to clients) until scroll completes !!!

	if (scroll && millis() - t > SCROLL_RATE * 1000)
	{
		t = millis();

		matrix.beginDraw();
		matrix.stroke(0xFFFFFFFF);
		matrix.textScrollSpeed(100);			// higher number is slower
		matrix.textFont(Font_5x7);

		// 0,1: x,y offset from top left corner
		// 0xFFFFFF: default, red color only
		matrix.beginText(0, 1, 0xFFFFFF);
		if (temp_status == 1)
		{
			matrix.println("	 COLD");	// triple space needed to fully show scrolling text
		}
		else if (temp_status == 2)
		{
			matrix.println("	 HOT");
		}
		else
		{
			matrix.println("	 OK");
		}
		matrix.endText(SCROLL_LEFT);
		matrix.endDraw();
	}
}



}