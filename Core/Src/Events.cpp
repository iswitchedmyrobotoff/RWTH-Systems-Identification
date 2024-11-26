#include "Events.h"

#include <string>
extern "C"
{
	#include "lcd/lcd.h"
}

extern TIM_HandleTypeDef htim4;
extern State currentState;

void Event::handleEvent(){}

Event::~Event(){}

void StartEvent::handleEvent()
{
	currentState = IDLE;
}

StartEvent::~StartEvent() {}

void ShowResultsEvent::handleEvent()
{
	currentState = SHOW_RESULTS;
}
ShowResultsEvent::~ShowResultsEvent(){}

void CancelEvent::handleEvent(){}
CancelEvent::~CancelEvent() {}

DisplayEvent::DisplayEvent(LCD_HandleTypeDef& lcd, std::string text): p_lLcd(lcd), p_sText(text){}

void DisplayEvent::handleEvent()
{
	std::string text1 = "";
	std::string text2 = "";

    if (p_sText.length() <= 16)
    {
        text1 = p_sText;
    }
    else
    {
        size_t midpoint = p_sText.length() / 2;

        size_t cut_position = midpoint;
        while (cut_position > 0 && p_sText[cut_position] != ' ')
        {
            --cut_position;
        }
        text1 = p_sText.substr(0, cut_position);
        text2 = p_sText.substr(cut_position + 1);
    }

	LCD_Clear(&p_lLcd);
	LCD_SetCursor(&p_lLcd, 0, 0);
	LCD_Printf(&p_lLcd, text1.c_str());
	LCD_SetCursor(&p_lLcd, 1, 0);
	LCD_Printf(&p_lLcd, text2.c_str());
}

DisplayEvent::~DisplayEvent() {}

TestEventLED::TestEventLED(TIM_HandleTypeDef& htim, LED led): p_tHtim(htim), p_lLED(led) {}

void TestEventLED::handleEvent()
{
	int red = 0;
	int green = 0;
	int blue = 0;

	if (p_lLED == RED){red = 255;}
	if (p_lLED == GREEN){green = 255;}
	if (p_lLED == BLUE){blue = 255;}

	__HAL_TIM_SET_COMPARE(&p_tHtim, TIM_CHANNEL_1, red);
	__HAL_TIM_SET_COMPARE(&p_tHtim, TIM_CHANNEL_3, green);
	__HAL_TIM_SET_COMPARE(&p_tHtim, TIM_CHANNEL_4, blue);
}

TestEventLED::~TestEventLED() {}

void TestEventGreenBlink::handleEvent()
{
	__HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1, 0);
	__HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_3, 0);
	__HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_4, 0);
}

TestEventGreenBlink::~TestEventGreenBlink(){}


void StartMeasureEvent::handleEvent()
{

}
StartMeasureEvent::~StartMeasureEvent() {}

void CalculationEvent::handleEvent() {}
CalculationEvent::~CalculationEvent() {}

void FinalCalculationEvent::handleEvent() {}
FinalCalculationEvent::~FinalCalculationEvent() {}
