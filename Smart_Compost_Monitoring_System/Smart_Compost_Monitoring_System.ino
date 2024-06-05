/**
 *
 *
 *
 *
 *
 *
 *		May.31.2024
 *		Santa Clara University
 *		ENGR 110: Community Based Engineering Design
 *		Smart Compost Monitoring System		for		Valley Verde, San Jose
 *		by Felix Fei
 *		https://www.linkedin.com/in/sichengfei/
 */





// need this to not include duplicate
#pragma once

#include <Arduino.h>
#include "WiFi_Server.h"
#include "LED_Matrix.h"
//#include "Storage.h"



// for temp and humidity sensor
#define SDA	18		// Serial Data Line
#define SCL 19		// Serial Clock Line

#undef	RX_BUFFER_DIM

using namespace std;





void serialSetUp()
{
	while (!Serial)
	{
		; // wait for serial port to connect. Needed for native USB port only
	}
	Serial.begin(9600); 	// Open serial connection to report values to host.
	Serial.println("Serial started.");
}





void setup()
{
	serialSetUp();
	Ntp::setup();
	Wifi::setup();
	Matrix::setup();

  // Wait for NTP server to connect.
  delay(1000);
  Ntp::checkOffline();

	Serial.println("All modules started.");
	Serial.println("Smart Compost Monitoring System by SCU for Valley Verde.");	
}





void loop()
{

	Ntp::loop();

	Sense::polling();

	Matrix::scrollingText();
	
	Wifi::loop();

	

	// Sync variables between different header files; since they can not include each other.

	Matrix::scroll = Wifi::scroll;
	Matrix::temp_status = (Matrix::TEMP_STATUS) (int) Sense::temp_status;
	
}





