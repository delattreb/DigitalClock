//
// Created by Bruno on 6/11/2017.
//

#include <Wire.h>
#include "var.h"
#include "libDS3231.h"
#include "libAdafruitNeoPixel.h"

// Var
Adafruit_NeoPixel ringled = Adafruit_NeoPixel(NBPIXELS_RINGLED, PIN_RINGLED, NEO_GRB + NEO_KHZ800);


int pix_hour, pix_hour_last, pix_minute, pix_minute_last, pix_seconde, pix_seconde_last, pix_dixseconde, pix_dixseconde_last;
unsigned long startmillis;
libDS3231 rtc;
RtcDateTime now;
int color = 0, mode = 0, light = 55;
boolean bfm_mode = false, bfm_color = false, bfm_light = false;

// Color
int colorTable[4][5][3] = {
{ { 0,0,255  },{ 0,0,255  },{ 220,0,255 },{ 255,40,40 },{ 0,0,5 } },
{ { 255,45,0 },{ 255,45,0 },{ 255,200,0 },{ 40,255,40 },{ 4,1,0 } },
{ { 0,255,45 },{ 0,255,45 },{ 0,200,200 },{ 150,150,0 },{ 0,4,1 } },
{ { 255, 0, 0 },{ 255,0,0 },{ 255,45,0  },{ 255,0,255 },{ 5,0,0 } } };
uint32_t color_hour;
uint32_t color_minute;
uint32_t color_seconde;
uint32_t color_dixseconde;
uint32_t color_cadran;

//
// setup
//
void setup() {
#ifdef DEBUG
	Serial.begin(BAUDS_RATE);
	while (!Serial) {
		;
	}
#endif 
	startmillis = millis();

	pinMode(PIN_MODE, INPUT);
	pinMode(PIN_COLOR, INPUT);
	pinMode(PIN_LIGHT, INPUT);

	setColor(color);
	rtc.init(false);

	// Init NeoPixel
	ringled.setBrightness(light);
	ringled.begin();
	ringled.show();

	//Initialze
	mode = 2;
	gethour(&pix_hour_last);
	pix_minute_last = now.Minute();
	pix_seconde_last = now.Second();
	pix_dixseconde_last = round(millis() / TIMEADJUST) % 60;
}

//
// loop
//
void loop() {
	readInput();
	setColor(color);
	ringled.setBrightness(light);
	display5();

	//Get time
	now = rtc.getDateTime();

	//Display time

	//Hour
	gethour(&pix_hour);
	if (pix_hour != pix_hour_last) {
		ringled.setPixelColor(pix_hour_last, 0);
		pix_hour_last = pix_hour;
	}
	ringled.setPixelColor(pix_hour, color_hour);

	//Minute
	pix_minute = now.Minute();
	if (pix_minute != pix_minute_last) {
		ringled.setPixelColor(pix_minute_last, 0);
		pix_minute_last = pix_minute;
	}
	ringled.setPixelColor(pix_minute, color_minute);

	//Seconde
	pix_seconde = now.Second();
	if (pix_seconde != pix_seconde_last) {
		ringled.setPixelColor(pix_seconde_last, 0);
		pix_seconde_last = pix_seconde;
	}
	if (mode > 0) {
		if (checkDisplayHM(pix_seconde))
			ringled.setPixelColor(pix_seconde, color_seconde);
	}

	//Dixseconde
	pix_dixseconde = round(millis() / TIMEADJUST) % 60;
	if ((pix_dixseconde != pix_dixseconde_last) && checkDisplayHM(pix_dixseconde) && checkDisplayS(pix_dixseconde)) {
		ringled.setPixelColor(pix_dixseconde_last, 0);
		pix_dixseconde_last = pix_dixseconde;
	}
	if (mode > 1) {
		if (checkDisplayHM(pix_dixseconde))
			ringled.setPixelColor(pix_dixseconde, color_dixseconde);
	}

	ringled.show();
}

//
// checkDisplayHM
//
boolean checkDisplayHM(int pix) {
	return !(pix == pix_hour || pix == pix_minute);
}

//
// checkDisplayS
//
boolean checkDisplayS(int pix) {
	return !(pix == pix_seconde);
}

//
// gethour
//
void gethour(int *pix_hour) {
	if (now.Hour() >= 12)
		*pix_hour = (now.Hour() - 12) * 5;
	else
		*pix_hour = now.Hour() * 5;
	*pix_hour += now.Minute() / 12;
}

//
// Display5
//
void display5()
{
	for (int i = 0; i <= 55; i += 5)
		ringled.setPixelColor(i, color_cadran);
}

//
// readInput
//
void readInput()
{
	//Load input mode
	if (digitalRead(PIN_MODE) == HIGH && !bfm_mode) {
		bfm_mode = true;
		mode += 1;
		if (mode > MAXMODE)
			mode = 0;
	}
	if (digitalRead(PIN_MODE) == LOW)
		bfm_mode = false;

	//Load input color 
	if (digitalRead(PIN_COLOR) == HIGH && !bfm_color) {
		bfm_color = true;
		color += 1;
		if (color > MAXCOLOR)
			color = 0;
	}
	if (digitalRead(PIN_COLOR) == LOW)
		bfm_color = false;

	//Load input light 
	if (digitalRead(PIN_LIGHT) == HIGH && !bfm_light) {
		bfm_light = true;
		light += INCREMENT;
		if (light > 255)
			light = INCREMENT;
	}
	if (digitalRead(PIN_LIGHT) == LOW)
		bfm_light = false;
}

//
// setColor
//
void setColor(int col)
{
	color_hour = ringled.Color(colorTable[col][0][0], colorTable[col][0][1], colorTable[col][0][2], GAMMA);
	color_minute = ringled.Color(colorTable[col][1][0], colorTable[col][1][1], colorTable[col][1][2], GAMMA);
	color_seconde = ringled.Color(colorTable[col][2][0], colorTable[col][2][1], colorTable[col][2][2], GAMMA);
	color_dixseconde = ringled.Color(colorTable[col][3][0], colorTable[col][3][1], colorTable[col][3][2], GAMMA);
	color_cadran = ringled.Color(colorTable[col][4][0], colorTable[col][4][1], colorTable[col][4][2], GAMMA);
}

