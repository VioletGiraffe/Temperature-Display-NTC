#include <Arduino.h>

#include <array>
#include <iterator>
#include <numeric>

#include <TM1638plus.h>

// GPIO I/O pins on the Arduino connected to strobe, clock, data, pick on any I/O pin you want.
#define STROBE_TM 5  // strobe = GPIO connected to strobe line of module
#define CLOCK_TM 6  // clock = GPIO connected to clock line of module
#define DIO_TM 7 // data = GPIO connected to data line of module
static constexpr bool high_freq = false; //default false, If using a high freq CPU > ~100 MHZ set to true.
TM1638plus tm(STROBE_TM, CLOCK_TM , DIO_TM, high_freq);

// https://learn.adafruit.com/thermistor/using-a-thermistor
// https://www.thinksrs.com/downloads/programs/Therm%20Calc/NTCCalibrator/NTCcalculator.htm

// thermistor-2.ino Intermediate test program for a thermistor. Adafruit Learning System Tutorial
// https://learn.adafruit.com/thermistor/using-a-thermistor by Limor Fried, Adafruit Industries
// MIT License - please keep attribution and please consider buying parts from Adafruit

void setup(void)
{
	tm.displayBegin();
	tm.brightness(0);
	//Serial.begin(115200);
	// connect AREF to 3.3V and use that as VCC, less noisy!
	analogReference(EXTERNAL);
}

static void displayFloat(float value)
{
	char buffer[64];
	dtostrf(value, 5, 2,  buffer);
	if (buffer[0] == ' ')
		buffer[0] = '0';
	buffer[TM_DISPLAY_SIZE] = '\0'; // In case of TM_DISPLAY_SIZE overrun, since dtostrf doesn't take the buffer length parameter
	tm.displayText(buffer);
}

void loop(void)
{
	// the value of the 'other' resistor
	static constexpr int SeriesResistorOhms = 15300.0f;


	// which analog pin to connect
	static constexpr auto ThermistorAnalogPin = A0;

	// how many samples to take and average, more takes longer
	// but is more 'smooth'
	std::array<int, 15> samples;
	// take N samples in a row, with a slight delay
	for (uint8_t i = 0; i < std::size(samples); ++i)
	{
		samples[i] = analogRead(ThermistorAnalogPin);
		delay(33);
	}

	// average all the samples out
	const auto averageAdcReading = static_cast<float>(std::accumulate(std::cbegin(samples), std::cend(samples), 0)) * (1.0f / std::size(samples));

	// convert the value to resistance
	const float R = SeriesResistorOhms / (1023.0f / averageAdcReading - 1.0f);

	constexpr float A = 0.9083717910e-3f;
	constexpr float B = 2.490174880e-4f;
	constexpr float C = 1.947702483e-7f;

	const float lnR = log(R);
	const float T = 1.0f / (A + B * lnR + C * pow(lnR, 3.0f)) - 273.15f /* Kelvin at 0 C */;
	
	// Serial.print("R: ");
	// Serial.println(R);
	// Serial.print("Temperature: ");
	// Serial.println(T);

	displayFloat(T);
}
