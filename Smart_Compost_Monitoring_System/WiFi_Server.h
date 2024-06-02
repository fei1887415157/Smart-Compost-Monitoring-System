#include "r_wdt_api.h"
#include <WiFiS3.h>
#include "Secret.h"
//#include "Storage.h"
#include <vector>
#include "Network_Time_Protocol.h"
#include "Sensor.h"
#include <String.h>





namespace Wifi
{



int status = WL_IDLE_STATUS;
WiFiServer server(80);

bool scroll = false;

bool AP_mode = false;





void printWifiStatus()
{
	// print the SSID of the network you're attached to:
	Serial.print("SSID: ");
	Serial.println(WiFi.SSID());

	// print your board's IP address:
	IPAddress ip = WiFi.localIP();
	Serial.print("IP Address: ");
	Serial.println(ip);

	// print the received signal strength:
	long rssi = WiFi.RSSI();
	Serial.print("signal strength (RSSI):");
	Serial.print(rssi);
	Serial.println(" dBm");

	// print where to go in a browser:
	Serial.print("To see this page in action, open a browser to http://");
	Serial.println(ip);
}



void setup()
{
	// check for the WiFi module:
	if (WiFi.status() == WL_NO_MODULE)
	{
		Serial.println("Communication with WiFi module failed!");
		exit(0);
	}

	String fv = WiFi.firmwareVersion();
	if (fv < WIFI_FIRMWARE_LATEST_VERSION)
	{
		Serial.println("Please upgrade the Arduino WiFi firmware.");
		exit(0);
	}



	if (AP_mode)		// Access Point Mode
	{
		WiFi.config(IPAddress(192,168,0,0));
		status = WiFi.beginAP(Secret::AP_SECRET_SSID, Secret::AP_SECRET_PASS);
		
		if (status != WL_AP_LISTENING)
		{
			Serial.println("Creating access point failed.");
			exit(0);
		}
	}
	else		// Station Mode
	{
		while (status != WL_CONNECTED)
		{
			Serial.print("Attempting to connect to SSID: ");
			Serial.println(Secret::STA_SECRET_SSID);

			status = WiFi.begin(Secret::STA_SECRET_SSID, Secret::STA_SECRET_PASS);

			delay(1000);
		}
	}
	


	// you're connected now, so print out the data
	Serial.print("You're connected to the network");
	printWifiStatus(); 

	server.begin();
}





void loop()
{
	// listen for incoming clients
	WiFiClient client = server.available();

	if (client)
	{
		Serial.print("New client connected...");

		if (client.connected())
		{
			int bytes = client.available();
			if (bytes > 0)
			{
				// read the whole http request
				String request = "";
				for (int i = 0; i < bytes; i++)
				{
					char c = client.read();
					request += c;
				}

				// print the http request, detailed info
				// Serial.println(request);				

				// parse http request 
				if (request.indexOf("GET /H") != -1)
				{
					scroll = true;

					// redirect to home page so refreshing the page would not cause bugs
					// too slow since loading the page twice
					//client.print("<head> <meta http-equiv=\"refresh\" content=\"0; url=http://192.168.1.186/\" /> </meta> </head>");
				}
				else if (request.indexOf("GET /L") != -1)
				{
					scroll = false;

					//client.print("<head> <meta http-equiv=\"refresh\" content=\"0; url=http://192.168.1.186/\" /> </meta> </head>");
				}

				else if (request.indexOf("GET /10s") != -1)
				{
					Sense::update_period = 10;
				}
				else if (request.indexOf("GET /1m") != -1)
				{
					Sense::update_period = 60;
				}
				else if (request.indexOf("GET /10m") != -1)
				{
					Sense::update_period = 600;
				}
				else if (request.indexOf("GET /1h") != -1)
				{
					Sense::update_period = 3600;
				}
				else if (request.indexOf("GET /8h") != -1)
				{
					Sense::update_period = 28800;
				}
				else if (request.indexOf("GET /24h") != -1)
				{
					Sense::update_period = 86400;
				}
				else if (request.indexOf("GET /ap") != -1)
				{
					AP_mode = true;
					Wifi::setup();
				}
				else if (request.indexOf("GET /sta") != -1)
				{
					AP_mode = false;
					Wifi::setup();
				}

	

				// send response 
				client.println("HTTP/1.1 200 OK");
				client.println("Content-type:text/html");

				// Important ! Do not delete this line !
				client.println();

				client.print("<html>");
				client.print("<style> table, th, td { border:1px solid black; } </style>");

				client.print("<body>");

				// &emsp;		4 spaces
				// <p style=\"font-size:3vw;\">		fixed font size
				
				client.print("<h3> Wi-Fi Mode: <a href=\"/ap\">New Network</a> &emsp; <a href=\"/sta\">Join a Network</a> </h3>");
				
				client.print("<h3> Arduino Temperature Status Display: <a href=\"/H\">On</a> &emsp; <a href=\"/L\">Off</a> </h3>");

				client.print("<h3> Sensor Polling Period: ");
				client.print("<a href=\"/10s\">10 Seconds</a> &emsp; <a href=\"/1m\">1 Minute</a> &emsp;");
				client.print("<a href=\"/10m\">10 Minutes</a> &emsp; <a href=\"/1h\">1 Hour</a> &emsp;");
				client.print("<a href=\"/8h\">8 Hours</a> &emsp; <a href=\"/24h\">24 Hours</a> &emsp; </h3>");

				client.print("<h3 style=\"display: inline;\"> Temperature Status: </h3>");

				

				switch (Sense::temp_status)
				{
					case Sense::ok:
						client.print("<b style=\"color:");
						client.print("Gray;\">"); //898b8f
						client.print("<h2> Acceptable </h2>");
						break;

					case Sense::cold:
						client.print("<font color=\"");
						client.print("075bed\">");
						client.print("<h2 style=\"display: inline;\"> Too Cold !!! </h2>");
						break;

					case Sense::hot:
						client.print("<font color=\"");
						client.print("de0b3c\">");
						client.print("<h2> Too Hot !!! </h2>");
						break;
				}

				client.print("</font color>");

				client.print("<br> </br>");

				client.print("<table style=\" width: 100%; text-align: center; \">");
				client.print("<tr>");
				client.print("<th> Date </th>");
				client.print("<th> Time </th>");
				client.print("<th> Temperature (F) </th>");
				client.print("<th> Relative Humidity (%) </th>");
				client.print("</tr>");

				int j = Sense::datestamp.size() - 1;
				for (int i = Sense::data_humidity.size() - 1; i > 0; i--)
				{
					client.print("<tr>");

					client.print("<td>");
					assert(j >= 0);
					if (i < Sense::datestamp.at(j).start_index) { j--; }
					client.print(Sense::datestamp.at(j).date);
					client.print("</td>");

					client.print("<td>");
					client.print(Sense::timestamp.at(i));
					client.print("</td>");

					client.print("<td>");
					client.print(Sense::data_temp_f.at(i));
					client.print("</td>");

					client.print("<td>");
					client.print(Sense::data_humidity.at(i));
					client.print("</td>");

					client.print("</tr>");
				}

				client.print("</table>");

				

				// Vector is LIFO. Thus most recent sensor reading is on top.
				// std::vector <int> v = Storage::readAll();
				// while ( ! v.empty() )
				// {
				// 	client.print(v.back());
				// 	client.print("<br>");
				// 	v.pop_back();
				// }

				client.print("</font>");
				client.print("</p>");
				client.print("</body>");
				client.print("</html>");
			}
		}
		
		// close the connection
		client.stop();
		Serial.println("Client disconnected");
	}
}


}