#include <SHT1x-ESP.h>
#include "Wire.h"
#include <String.h>





namespace Sense
{



#define MIN_TEMP_F 90
#define MAX_TEMP_F 140



// Default to 5.0v boards, such as Arduino UNO.
SHT1x sht10(SDA, SCL);

// If 3.3v board is used.
//SHT1x sht10(SDA, SCL, SHT1x::Voltage::DC_3_3v);



// Only store unique date and corresponding index range.
// Save space.
struct Date
{
    String date;
    int start_index;		// Marks the start index (inclusive) of this date,
							// out of all total readings.

    Date(String d, int divide) : date(d), start_index(divide) {}
};



static std::vector <float> data_temp_f = { };
static std::vector <float> data_humidity = { };

static std::vector <Date> datestamp = { };		// storing unique date to save space
static std::vector <String> timestamp = { };



enum TEMP_STATUS
{
	ok = 0,
	cold = 1,
	hot = 2
};
TEMP_STATUS temp_status = ok;

float temp_c;
float temp_f;
float humidity;



// Clear all data before maxing out 32 KB RAM and 256 KB Flash, otherwise the program will hang.
// Important when sensor polling is frequent, such as 1 min or less.
void clearAllData()
{
	data_temp_f.clear();
	data_humidity.clear();
	datestamp.clear();
	timestamp.clear();
}


void checkTemp()
{
	// check temp range
	if (temp_f < MIN_TEMP_F)
	{
		temp_status = cold;
	}
	else if (temp_f > MAX_TEMP_F)
	{
		temp_status = hot;
	}
	else
	{
		temp_status = ok;
	}
}



/*	Time in seconds before updating sensor readings.
Frequent reading will overflow RAM, causing the program to hang.
0+9+32+32 = 73 bits per reading		assume datestamp space negligible
RAM: 32 kB = 32000 bytes = 256000 bits
256000/73 = 3506 readings
1 reading per day 		->	3506 days
		  per hour		->	3506/24 	= 146 days
		  per 10 minute	->	146/60*10 	= 24 days
		  per minute	->  146/60 		= 2.4 days
		  per 10 second	->	2.4/60*24 	= 9.6 hours
*/
long int update_period = 10;

int t = 0;

void polling()
{
	// -1 to adjust for processing delay
	if ((millis() - t) / 1000 > update_period - 1)
	{
		t = millis();

		// Read values from the sensor.
		temp_c = sht10.readTemperatureC();
		temp_f = sht10.readTemperatureF();
		humidity = sht10.readHumidity();

		checkTemp();

		data_temp_f.push_back(Sense::temp_f);
		data_humidity.push_back(Sense::humidity);

		// testing Date custom data structure
		//Serial.println(datestamp.size());

		String time = Ntp::getTime();

		// Only add current date if it is a new date.
		// Start at index 0.
		if (datestamp.empty())
		{
			datestamp.push_back(Date(Ntp::getDate(), 0));
		}
		// New date.
		else if (datestamp.back().date != Ntp::getDate())
		{
			datestamp.push_back(Date(Ntp::getDate(), data_humidity.size()));
		}
		else
		{
			// Do nothing, save space.
		}

		timestamp.push_back(time);



		// Print the values to the serial port.
		// Serial.print("Temperature: ");
		// Serial.print(temp_c, DEC);
		// Serial.print("C / ");
		// Serial.print(temp_f, DEC);
		// Serial.print("F. Humidity: ");
		// Serial.print(humidity);
		// Serial.println("%");
	}
}



}
