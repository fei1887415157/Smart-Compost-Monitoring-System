/*
 * Not currently using.
 */



#include <vector>
#include <EEPROM.h>



namespace Storage
{



static int current_index = 0;



void clearAll()
{
	/***
		Iterate through each byte of the EEPROM storage.
		Larger AVR processors have larger EEPROM sizes, E.g:
		- Arduino Duemilanove: 512 B EEPROM storage.
		- Arduino Uno:				 1 kB EEPROM storage.
		- Arduino Mega:				4 kB EEPROM storage.
		Rather than hard-coding the length, you should use the pre-provided length function.
		This will make your code portable to all AVR processors.
	***/

	for (int i = 0; i < EEPROM.length(); i++)
	{
		EEPROM.update(i, 0);
	}

	current_index = 0;
}



// Find the next empty index to write data.
// Must call once during main setup().
void setup()
{
	int i = 0;
	while (i < EEPROM.length() && EEPROM.read(i) != 0)
	{
		i++;
	}

	if (i == EEPROM.length())
	{
		Serial.println("EEPROM capacity full. Overwriting oldest data.");
		i = 0;
	}
	
	current_index = i;
	Serial.print("Next empty index: ");
	Serial.println(current_index);
}



// Only write 8-bit unsigned int: 0 to 255.
void write(int data)
{
	// Only write if data is different. EEPROM write limited to 100,000.
	EEPROM.update(current_index ++, data);
}



// Return all stored data.
std::vector <int> readAll()
{
	std::vector <int> all = { };

	// The first 0 indicates the end of stored data.
	for (int i = 0; i < EEPROM.length() && EEPROM.read(i) != 0; i++)
	{
		all.push_back(EEPROM.read(i));
	}

	return all;
}



}