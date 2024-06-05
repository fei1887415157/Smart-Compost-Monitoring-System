#include "NTP.h"
#include <WiFiS3.h>
#include <String.h>



// Internet is required at start up to get current date and time, and to update.
// Other times Internet is not required for the time counter to run.



namespace Ntp
{



static WiFiUDP wifiUdp;
static NTP ntp(wifiUdp);

bool offline = false;



void checkOffline()
{
	// Time is updated from the NTP server every 60s.
	ntp.update();

	Serial.print("Current date and time: ");
	Serial.print(ntp.formattedTime("%B.%d.%Y "));
	Serial.println(ntp.formattedTime("%T"));
	if (strcmp(ntp.formattedTime("%B.%d.%Y"), "December.31.1969") == 0)
	{
		offline = true;
		Serial.println("Network Time Protocol is offline.");
		Serial.println("");
	}
	else
	{
		offline = false;
		Serial.println("Network Time Protocol is online.");
		Serial.println("");
	}
}



String getDate()
{
	if (offline)
	{
		return "";
	}
	else
	{
		// %B: month	%d: day		%Y: year
		return ntp.formattedTime("%B.%d.%Y");
	}

	// testing Date custom data structure
	//return ntp.formattedTime("%T");
}



String getWeekday()
{
	if (offline)
	{
		return "";
	}
	else
	{
		// %A: weekday
		return ntp.formattedTime("%A");
	}
}



String getTime()
{
	if (offline)
	{
		return "";
	}
	else
	{
		// %T: time h:m:s
		return ntp.formattedTime("%T");
	}
}



void setup()
{
	// PDT (Pacific Daylight Time) begins on second Sunday in March at 2AM local time.
	// 7 hours behind UTC (Coordinated Universal Time).
	ntp.ruleDST("PDT", Second, Sun, Mar, 2, -7 * 60);

	// PST (Pacific Standard Time) begins on first Sunday in November at 2AM local time.
	// 8 hours behind UTC (Coordinated Universal Time).
	ntp.ruleSTD("PST", First, Sun, Nov, 2, -8 * 60);

	ntp.begin();

	Serial.println("Network Time Protocol started.");
}


// in minutes
#define OFFLINE_RETRY_RATE 1
#define ONLINE_RETRY_RATE 60*24
unsigned long interval = 0, counter = 0;

void loop()
{
	counter = millis() / 60000;

	if (offline)
	{
		if (counter - interval > OFFLINE_RETRY_RATE)
		{
			interval = counter;

			checkOffline();

			// %B: month	%d: day		%Y: year	%A: weekday		%T: time h:m:s
			// Serial.println(ntp.formattedTime("%A %B.%d.%Y"));
			// Serial.println(ntp.formattedTime("%T"));
		}
	}

	else
	{
		if (counter - interval > ONLINE_RETRY_RATE)
		{
			interval = counter;

			checkOffline();

			// %B: month	%d: day		%Y: year	%A: weekday		%T: time h:m:s
			// Serial.println(ntp.formattedTime("%A %B.%d.%Y"));
			// Serial.println(ntp.formattedTime("%T"));
		}
	}
}



}