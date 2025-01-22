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


class WaitEvent: public Event
{
private:
  int p_dWaitTime = 0;
public:
  WaitEvent(int waitTime);
  void handleEvent() override;
  ~WaitEvent();
};


class DisplayEvent: public Event
{
private:
  LCD_HandleTypeDef& p_lLcd;
  std::string p_sText1 = "";
  std::string p_sText2 = "";

public:
  DisplayEvent(LCD_HandleTypeDef& lcd, std::string text1 = "", std::string text2 = "");
  void handleEvent() override;
  ~DisplayEvent();
};


class TestEventLED: public Event
{
private:
  TIM_HandleTypeDef& p_tHtim;
  LED p_lLED;

public:
  TestEventLED(TIM_HandleTypeDef& htim, LED led);
  void handleEvent() override;
  ~TestEventLED();
};


class CalculationEvent: public Event
{
public:
  void handleEvent() override;
  ~CalculationEvent();
};


class MeasurementEvent: public Event
{
private:
  TIM_HandleTypeDef& p_tHtim;
  ADC_HandleTypeDef& p_aHadc;
public:
  MeasurementEvent(TIM_HandleTypeDef& htim, ADC_HandleTypeDef& hadc);
  void handleEvent() override;
  ~MeasurementEvent();
};


class FastMeasurementEvent: public Event
{
private:
  TIM_HandleTypeDef& p_tHtim;
  ADC_HandleTypeDef& p_aHadc;
public:
  FastMeasurementEvent(TIM_HandleTypeDef& htim, ADC_HandleTypeDef& hadc);
  void handleEvent() override;
  ~FastMeasurementEvent();
};


class ComparisonEvent: public Event
{
public:
  void handleEvent() override;
  ~ComparisonEvent();
};


#endif
