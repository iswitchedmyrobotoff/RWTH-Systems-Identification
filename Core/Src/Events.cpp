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
extern volatile State currentState;
extern volatile uint8_t buttonPressed; // UPDATE


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


// UPDATE: Warteprozess
// Zu Anfang buttonPressed = 0. Die Schleife wird abgebrochen, wenn buttonPressed = 1
/**
 * @brief Zeigt Ergebnisse an und wartet auf Benutzereingabe zum Fortfahren.
 *
 * Setzt den aktuellen Zustand auf `SHOW_RESULTS` und blockiert für maximal 2500 ms,
 * während auf eine Benutzereingabe gewartet wird. Die Schleife wird abgebrochen,
 * sobald die Variable `buttonPressed` auf `1` gesetzt wird.
 *
 * @note Die Methode dient dazu, eine Pause einzulegen, bis der Benutzer interagiert.
 */
void ShowResultsEvent::handleEvent()
{
  currentState = SHOW_RESULTS;
  buttonPressed = 0;
  for (int i = 0; i < 2500; i++)
  {
      if (buttonPressed)
      {
              return;
      }
      HAL_Delay(1);
  }
}

ShowResultsEvent::~ShowResultsEvent(){}


// UPDATE: Warteprozess (nur zum Beobachten, wird später gelöscht)
/**
 * @brief
 * Führt einen Verzögerungsvorgang aus.
 *
 * Diese Methode blockiert für eine definierte Dauer (2500 ms) durch eine Schleife
 * mit kurzen Verzögerungen. Sie dient dazu, eine Unterbrechung oder Wartezeit
 * zwischen Aktionen zu simulieren.
 *
 * @note Die Methode verwendet HAL_Delay() für die Verzögerung.
 */
void CancelEvent::handleEvent()
{
  for (int i = 0; i < 2500; i++)
  {
    HAL_Delay(1);
  }
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


void StartMeasureEvent::handleEvent(){}
StartMeasureEvent::~StartMeasureEvent() {}


// UPDATE: Warteprozess
// Zu Anfang buttonPressed = 0. Die Schleife wird abgebrochen, wenn buttonPressed = 1
/**
 * @brief Führt eine Berechnungsverzögerung aus, die durch Benutzereingaben abgebrochen werden kann.
 *
 * Diese Methode blockiert für maximal 2500 ms durch kurze Verzögerungen, kann jedoch
 * vorzeitig beendet werden, wenn die Variable `buttonPressed` auf `1` gesetzt wird.
 * Sie ermöglicht eine Wartezeit, die auf Benutzereingaben reagiert.
 *
 * @note Die Schleife überprüft in regelmäßigen Abständen den Status von `buttonPressed`.
 */
void CalculationEvent::handleEvent()
{
  buttonPressed = 0;
  for (int i = 0; i < 2500; i++)
  {
    if (buttonPressed){return;}
    HAL_Delay(1);
  }
}

CalculationEvent::~CalculationEvent(){}


void FinalCalculationEvent::handleEvent() {}
FinalCalculationEvent::~FinalCalculationEvent() {}
