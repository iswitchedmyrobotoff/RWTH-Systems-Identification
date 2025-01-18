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
extern TIM_HandleTypeDef htim3;
//extern LCD_HandleTypeDef lcd;
extern volatile State currentState;
extern volatile uint8_t buttonPressed;
//extern uint16_t capacity;


void Event::handleEvent(){}
Event::~Event(){}

/**
 * @brief Startevent, bei welchem am Anfang ein IDLE-state ausgeführt wird.
 */
void StartEvent::handleEvent()
{
	currentState = IDLE;
}

StartEvent::~StartEvent() {}


/**
 * @brief Zeigt Ergebnisse an.
 *
 * Setzt den aktuellen Zustand auf `SHOW_RESULTS`.
 *
 */
void ShowResultsEvent::handleEvent()
{
  currentState = SHOW_RESULTS;
}

ShowResultsEvent::~ShowResultsEvent(){}



void CancelEvent::handleEvent()
{

}

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

  if (p_lLED == BLINKING){HAL_TIM_Base_Start_IT(&htim3);}
  else{HAL_TIM_Base_Stop_IT(&htim3);}
}

TestEventLED::~TestEventLED() {}


//void ChooseCapacityEvent::handleEvent()
//{
//  buttonPressed = 0;
//  uint16_t blackButtonPressed = 1;
//  std::string msg;
//  capacity = 10;
//
//  msg = "{10} 100  1000 ";
//	//  strcpy(capacity, "100uF     ");
//  LCD_Clear(&lcd);
//  LCD_SetCursor(&lcd, 0, 0);
//  LCD_Printf(&lcd, "Capacity (uF): ");
//  LCD_SetCursor(&lcd, 1, 0);
//  LCD_Printf(&lcd, msg.c_str());
//	  while(1)
//	  {
//		  if (HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_9) == GPIO_PIN_RESET)
//		  {
//			  if (blackButtonPressed == 1)
//			  {
//				msg = " 10 {100} 1000 ";
//	  //			  strcpy(capacity, "100uF     ");
//				blackButtonPressed++;
//				if (buttonPressed){return;}
//
//			  }
//			  else if (blackButtonPressed == 2)
//			  {
//				msg = " 10  100 {1000}";
//	  //			  strcpy(capacity, "1000uF    ");
//				blackButtonPressed++;
//				if (buttonPressed){return;}
//			  }
//			  else if (blackButtonPressed == 3)
//			  {
//				msg = "{10} 100  1000 ";
//	  //			  strcpy(capacity, "10uF      ");
//				blackButtonPressed = 1;
//				if (buttonPressed){return;}
//			  }
//		  }
//		  LCD_SetCursor(&lcd, 1, 0);
//		  if (buttonPressed){return;}
//		  LCD_Printf(&lcd, msg.c_str());
//		  if (buttonPressed){return;}
//		  for (int i = 0; i < 100; i++)
//		  {
//			if (buttonPressed){return;}
//			HAL_Delay(1);
//		  }
//	  }
//}
//ChooseCapacityEvent::~ChooseCapacityEvent() {}



void CalculationEvent::handleEvent()
{
}

CalculationEvent::~CalculationEvent(){}


void FinalCalculationEvent::handleEvent() {}
FinalCalculationEvent::~FinalCalculationEvent() {}


// Die Doxygen Kommentare können aktualisiert werden.
/**
 * @brief
 * Führt einen Verzögerungsvorgang aus, die durch Benutzereingaben abgebrochen werden kann.
 *
 * Diese Methode blockiert für eine definierte Dauer (p_dWaitTime/2 ms) durch eine Schleife
 * mit kurzen Verzögerungen, kann jedoch vorzeitig beendet werden, wenn die Variable
 * `buttonPressed` auf `1` gesetzt wird. Sie ermöglicht eine Wartezeit, die auf
 * Benutzereingaben reagiert.
 *
 * @note Die Methode verwendet HAL_Delay() für die Verzögerung und überprüft in
 * regelmäßigen Abständen den Status von `buttonPressed`.
 */
WaitEvent::WaitEvent(int waitTime): p_dWaitTime(waitTime) {}


void WaitEvent::handleEvent()
{
  buttonPressed = 0;
  for (int i = 0; i < p_dWaitTime/2; i++)
  {
	if (buttonPressed){return;}
	HAL_Delay(1);
  }
}

WaitEvent::~WaitEvent(){}
