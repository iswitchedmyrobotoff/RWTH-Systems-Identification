/**
 * @file Events.h
 * @brief Events.h ist die Header-Datei zu Events.cpp
 */
#ifndef EVENTS_H
#define EVENTS_H
#include <iostream>
#include "Enums.h"

extern "C"
{
	#include "lcd/lcd.h"
}



class Event
{
public:
	virtual void handleEvent();
	virtual ~Event();
};

class StartEvent: public Event
{
public:
	void handleEvent() override;
	~StartEvent();
};

class ShowResultsEvent: public Event
{
public:
	void handleEvent() override;
	~ShowResultsEvent();
};

class CancelEvent: public Event
{
public:
	void handleEvent() override;
	~CancelEvent();
};

class DisplayEvent: public Event
{
private:
	LCD_HandleTypeDef p_lLcd;
	std::string p_sText = "";

public:
	DisplayEvent(LCD_HandleTypeDef& lcd, std::string text);
	void handleEvent() override;
	~DisplayEvent();
};

class TestEventLED: public Event
{
private:
	TIM_HandleTypeDef p_tHtim;
	LED p_lLED;
public:
	TestEventLED(TIM_HandleTypeDef& htim, LED led);
	void handleEvent() override;
	~TestEventLED();
};

// UPDATE: nicht mehr benötigt, kann gelöscht werden
/*
class TestEventGreenBlink: public Event
{
public:
	void handleEvent() override;
	~TestEventGreenBlink();
};
*/



class StartMeasureEvent: public Event
{
public:
	void handleEvent() override;
	~StartMeasureEvent();
};


class CalculationEvent: public Event
{
public:
	void handleEvent() override;
	~CalculationEvent();
};

class FinalCalculationEvent: public Event
{
public:
	void handleEvent() override;
	~FinalCalculationEvent();
};


#endif
