#include <Arduino.h>

#include <iterator>
#include <numeric>

// https://learn.adafruit.com/thermistor/using-a-thermistor
// https://www.thinksrs.com/downloads/programs/Therm%20Calc/NTCCalibrator/NTCcalculator.htm

// thermistor-2.ino Intermediate test program for a thermistor. Adafruit Learning System Tutorial
// https://learn.adafruit.com/thermistor/using-a-thermistor by Limor Fried, Adafruit Industries
// MIT License - please keep attribution and please consider buying parts from Adafruit

// which analog pin to connect
static constexpr auto ThermistorAnalogPin = A0;
// how many samples to take and average, more takes longer
// but is more 'smooth'
static constexpr int SamplessToAverage = 5;
// the value of the 'other' resistor
static constexpr int SeriesResistorOhms = 10000;

int samples[SamplessToAverage];

void setup(void)
{
	Serial.begin(9600);
	// connect AREF to 3.3V and use that as VCC, less noisy!
	analogReference(EXTERNAL);
}

void loop(void)
{
	// take N samples in a row, with a slight delay
	for (int i = 0; i < SamplessToAverage; ++i)
	{
		samples[i] = analogRead(ThermistorAnalogPin);
		delay(10);
	}

	// average all the samples out
	const int averageAdcReading = std::accumulate(std::cbegin(samples), std::cend(samples), 0) / SamplessToAverage;

	Serial.print("Average analog reading: ");
	Serial.println(averageAdcReading);
	// convert the value to resistance
	const float R = SeriesResistorOhms / (1023.0f / averageAdcReading - 1.0f);

	Serial.print("Thermistor resistance: ");
	Serial.println(R);

	constexpr float A = 0.9083717910e-3f;
	constexpr float B = 2.490174880e-4f;
	constexpr float C = 1.947702483e-7f;

	const float lnR = log(R);
	const float T = 1.0f / (A + B * lnR + C * pow(lnR, 3.0f));
	
	Serial.print("Temperature: ");
	Serial.println(T);

	delay(1000);
}
