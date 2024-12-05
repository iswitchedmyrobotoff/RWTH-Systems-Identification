/**
 * @file Events.cpp
 * @brief Events.cpp enthält Evente für zb. das Display
 */

#include "Events.h"

#include <string>
extern "C"
{
	#include "lcd/lcd.h"
}

//extern TIM_HandleTypeDef htim4; UPDATE: nicht mehr benötigt, kann gelöscht werden
extern TIM_HandleTypeDef htim3; // UPDATE: Interrupt fürs Blinken
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
/**
 * @brief DisplayEvent
 *
 *@details
 * -Sorgt für die Ausgabe des Textes auf dem LCD Display
 * -Wenn genügend Platz auf dem Display vorhanden ist, dann gebe den Text aus
 * ansonsten gebe die Hälfte aus. Bei mehr als 32 gibt es einen Fehler
 * -c_str macht einen Konstanten String aus den text-Varriablen
 *
 * @param text1 string, welcher den ersten Teil des Textes enthält
 * @param text2 string, welcher den zweiten Teil des Textes enthält
 * @param p_sTest string member Varriable von der Klasse
 * @param midpoint enthält den geteilten Text
 *
 */
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


/**
 * @brief Verarbeitet den aktuellen LED-Zustand und aktualisiert die entsprechenden PWM-Werte.
 *
 * Diese Funktion passt die RGB-Kanäle einer LED basierend auf dem Zustand `p_lLED` an.
 * Zusätzlich wird ein Timer für den Blinkmodus gestartet oder gestoppt, falls erforderlich.
 *
 * @details
 * - Setzt die PWM-Werte für die Farbkanäle Rot, Grün und Blau entsprechend des ausgewählten LED-Zustands.
 * - Falls der LED-Zustand `BLINKING` ist, wird ein Timer-Interrupt gestartet, um die Blinkfunktion zu aktivieren.
 * - Für andere Zustände wird sichergestellt, dass der Timer deaktiviert ist.
 *
 * @note Der LED-Zustand (`p_lLED`) muss einer der vordefinierten Zustände sein: `RED`, `GREEN`, `BLUE`, `BLINKING`.
 *
 * @param Keine
 * @retval Keine
 */
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

	// UPDATE: Starten des Timers fürs Blinken, Stoppen für andere LED-Zustände
	if (p_lLED == BLINKING){HAL_TIM_Base_Start_IT(&htim3);}
	else{HAL_TIM_Base_Stop_IT(&htim3);}
}

TestEventLED::~TestEventLED() {}

// UPDATE: nicht mehr benötigt, kann gelöscht werden
/*
void TestEventGreenBlink::handleEvent()
{
	__HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1, 0);
	__HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_3, 0);
	__HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_4, 0);
}
*/

// TestEventGreenBlink::~TestEventGreenBlink(){} UPDATE: nicht mehr benötigt, kann gelöscht werden 


void StartMeasureEvent::handleEvent()
{

}
StartMeasureEvent::~StartMeasureEvent() {}

void CalculationEvent::handleEvent() {}
CalculationEvent::~CalculationEvent() {}

void FinalCalculationEvent::handleEvent() {}
FinalCalculationEvent::~FinalCalculationEvent() {}
