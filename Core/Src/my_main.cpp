#include "my_main.h"
#include <string>
#include <queue>
#include <stdio.h>
#include <memory>
#include "Events.h"
#include "Enums.h"
extern "C"
{
  #include "lcd/lcd.h"
}

// my_main(htim4, hi2c2)
// HAL_TIM_PeriodElapsedCallback_(htim, htim3)
// HAL_GPIO_EXTI_Callback_(uint16_t GPIO_Pin, htim4)

extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim4;
extern I2C_HandleTypeDef hi2c2;
extern ADC_HandleTypeDef hadc2;

#define GPIO_PIN_MAIN_BUTTON GPIO_PIN_13
#define GPIO_PIN_BLACK_BUTTON GPIO_PIN_9
#define GPIO_PIN_BLUE_BUTTON GPIO_PIN_11
#define DEBOUNCE_THRESHOLD 300 // ms

LCD_HandleTypeDef lcd;

volatile State currentState = IDLE;
volatile uint32_t currentTime;
volatile uint32_t lastButtonPressTime;
volatile uint32_t currentTime2;
volatile uint32_t lastButtonPressTime2;
volatile uint32_t currentTime3;
volatile uint32_t lastButtonPressTime3;
volatile uint8_t buttonPressed = 0; // to control if the button has been pressed for delay
volatile double capacity = 10; // uF
volatile double resistance = 1; // kOhm
volatile uint16_t blueButtonPressed = 1; // to control how many times the blue button has been pressed
uint16_t resUnknown = 0; // to control if the resistance is unknown (e.g. potentiometer is used) or not
uint32_t numPeriods = 0;
uint32_t maxPeriods = 2600;

//! Hier wird ein Objekt event_queue erstellt
std::queue<std::unique_ptr<Event>> event_queue;

/**
 * @brief Löscht alle Elemente aus der Warteschlange.
 */
void clearQueue()
{
  while (!event_queue.empty()){event_queue.pop();}
}


/**
 * @brief Interrupt fürs Blinken
 * @details
 * - Beim pwmValue = 255 -> LED wird ausgeschaltet
 * - Beim pwmValue = 0 -> LED wird angeschaltet
 *
 * @param htim
 */
void HAL_TIM_PeriodElapsedCallback_(TIM_HandleTypeDef *htim)
{
  if (htim == &htim3)
  {
	if (numPeriods >= maxPeriods)
	{
      event_queue.push(std::make_unique<DisplayEvent>(lcd, "Unexpected delay", "in the operation."));
      event_queue.push(std::make_unique<WaitEvent>(2000));
	  HAL_GPIO_EXTI_Callback_(GPIO_PIN_MAIN_BUTTON);
	}
    uint32_t pwmValue = __HAL_TIM_GET_COMPARE(&htim4, TIM_CHANNEL_3);
    if (pwmValue == 255){pwmValue = 0;}
    else if (pwmValue == 0){pwmValue = 255;}
    __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_3, pwmValue);
    numPeriods++;
  }
}


/**
 * @brief ınterrupt, welcher den Operierenden Betrieb enthält
 *
 * @details Somit wird alles was beim operieren ausgelößt werden soll, hier aufgerufen
 *
 * Ebenfalls gibt es ein printf mit einem float um die convertierung von c zu cpp zu überprüfen.
 *
 * @details Nachdem das Operieren beendet wurde, werden die Resultate angezeigt und danach wird die Queue berreinigt.
 *
 * @param lastButtonPressTime enthält die Zeit, zu welcher der Button das letzte mal gedrückt wurde.
 * @param currentTime enthält die aktuelle Zeit
 * @param GPIO_Pin
 */
void HAL_GPIO_EXTI_Callback_(uint16_t GPIO_Pin)
{
  if (GPIO_Pin == GPIO_PIN_MAIN_BUTTON)
  {
    currentTime = HAL_GetTick();
    if (currentTime - lastButtonPressTime > DEBOUNCE_THRESHOLD)
    {
      if (currentState == IDLE)
      {
    	  currentState = ASK_CAPACITOR;
    	  capacity = 10;
    	  event_queue.push(std::make_unique<TestEventLED>(htim4, BLINKING));
    	  event_queue.push(std::make_unique<DisplayEvent>(lcd, "Capacity (uF):", "{10} 100  1000 "));
      }
      else if (currentState == READY)
      {
        buttonPressed = 1;
        currentState = OPERATING;
        std::string msg1 = "C = " + std::to_string(static_cast<int>(capacity)) + "uF ";
        std::string msg2 = "R = " +
        		(resUnknown ? "Unknown" : std::to_string(static_cast<int>(resistance)) + "kOhm");
        event_queue.push(std::make_unique<DisplayEvent>(lcd, msg1, msg2));
        event_queue.push(std::make_unique<WaitEvent>(2000));
        event_queue.push(std::make_unique<DisplayEvent>(lcd, "Calculating the", "time constant..."));
        event_queue.push(std::make_unique<CalculationEvent>());
//        event_queue.push(std::make_unique<MeasurementEvent>(htim1, hadc2));
        event_queue.push(std::make_unique<FastMeasurementEvent>(htim1, hadc2));
        event_queue.push(std::make_unique<ComparisonEvent>());
        event_queue.push(std::make_unique<ShowResultsEvent>());
        event_queue.push(std::make_unique<TestEventLED>(htim4, BLUE));
        event_queue.push(std::make_unique<DisplayEvent>(lcd));
        event_queue.push(std::make_unique<StartEvent>());
        event_queue.push(std::make_unique<TestEventLED>(htim4, GREEN));
        event_queue.push(std::make_unique<DisplayEvent>(lcd, "Press the button", "to start."));
      }
      else if (currentState == OPERATING || currentState == SHOW_RESULTS
    		  || currentState == ASK_CAPACITOR || currentState == ASK_RESISTANCE)
      {
        buttonPressed = 1;
        currentState = CLEANING_UP;
        clearQueue();
        event_queue.push(std::make_unique<TestEventLED>(htim4, RED));
        event_queue.push(std::make_unique<DisplayEvent>(lcd, "Operation is", "aborted."));
        event_queue.push(std::make_unique<CancelEvent>());
        event_queue.push(std::make_unique<WaitEvent>(5000));
        event_queue.push(std::make_unique<StartEvent>());
        event_queue.push(std::make_unique<TestEventLED>(htim4, GREEN));
        event_queue.push(std::make_unique<DisplayEvent>(lcd, "Press the button", "to start."));
      }
      lastButtonPressTime = currentTime;
    }
  }
  else if (GPIO_Pin == GPIO_PIN_BLUE_BUTTON)
  {
	currentTime2 = HAL_GetTick();
	if (currentTime2 - lastButtonPressTime2 > DEBOUNCE_THRESHOLD)
	{
	  std::string msg;
	  if (currentState == ASK_CAPACITOR)
	  {
		switch (blueButtonPressed)
		{
		case 1:
			msg = " 10 {100} 1000 ";
			// msg = "100uF";
			capacity = 100;
			blueButtonPressed = 2;
			break;
		case 2:
		  msg = " 10  100 {1000}";
		  // msg = "1000uF";
		  capacity = 1000;
		  blueButtonPressed = 3;
		  break;
		case 3:
		  msg = "{10} 100  1000 ";
		  // msg = "10uF";
		  capacity = 10;
		  blueButtonPressed = 1;
		  break;
		}
	    event_queue.push(std::make_unique<DisplayEvent>(lcd, "Capacity (uF): ", msg));
	  }
	  else if (currentState == ASK_RESISTANCE)
	  {
		switch (blueButtonPressed)
		{
		case 1:
		  blueButtonPressed = 2;
		  resistance = 2;
		  break;
		case 2:
		  blueButtonPressed = 3;
		  resistance = 3;
		  break;
		case 3:
		  blueButtonPressed = 4;
		  resistance = 4;
		  break;
		case 4:
		  blueButtonPressed = 5;
		  resistance = 5;
		  break;
		case 5:
		  blueButtonPressed = 6;
		  resistance = 6;
		  break;
		case 6:
		  blueButtonPressed = 7;
		  resistance = 7;
		  break;
		case 7:
		  blueButtonPressed = 8;
		  resistance = 8;
		  break;
		case 8:
		  blueButtonPressed = 9;
		  resistance = 9;
		  break;
		case 9:
		  blueButtonPressed = 10;
		  resistance = 10;
		  break;
		case 10:
		  blueButtonPressed = 11;
		  resistance = 0;
		  break;
		case 11:
		  blueButtonPressed = 1;
		  resistance = 1;
		  break;
		}
		if (resistance == 0)
		{
	      resUnknown = 1;
	      event_queue.push(std::make_unique<DisplayEvent>(lcd, "Resistance: ", "Unknown"));
		}
		else
		{
		  resUnknown = 0;
		  event_queue.push(std::make_unique<DisplayEvent>(lcd, "Resistance:", "{"
							+ std::to_string((int)resistance) + "}kOhm"));
		}
	  }

	  lastButtonPressTime2 = currentTime2;
	}

  }
  else if (GPIO_Pin == GPIO_PIN_BLACK_BUTTON)
  {
    currentTime3 = HAL_GetTick();
	if (currentTime3 - lastButtonPressTime3 > DEBOUNCE_THRESHOLD)
	{
	  if (currentState == ASK_CAPACITOR)
	  {
	    blueButtonPressed = 11;
	    currentState = ASK_RESISTANCE;
	    HAL_GPIO_EXTI_Callback(GPIO_PIN_BLUE_BUTTON);
	  }
	  else if (currentState == ASK_RESISTANCE)
	  {
	    blueButtonPressed = 1;
	    currentState = READY;
	    HAL_GPIO_EXTI_Callback(GPIO_PIN_MAIN_BUTTON);
	  }
	  lastButtonPressTime3 = currentTime3;
	}

  }


}

/**
 * @brief  The application entry point.
 *  Hier ist das Projekt festgehalten. Zusätzlich werden hier die Interrups aufgerufen und das
 *  Programm an sich ausgeführt. Hier erfolgt ebenfalls die abfolge mit dem drücken des Buttons,
 *  welcher ein Event auslöst und so die Mesung startet
 * @retval int
 */
void my_main()
{
  /**
   * @defgroup Initalisierung_des_Displays
   * @brief Initialisiert die Hardwarekomponenten und das Event-System.
   *
   * @details Dieser Abschnitt des Codes richtet die LCD-Anzeige, die PWM-Kanäle und die
   * Ereigniswarteschlange ein. Er stellt sicher, dass die Hardware bereit ist und das
   * System in einen definierten Ausgangszustand versetzt wird.
   * Die Ereignisse in der Warteschlange starten anschließend die Anwendung.
   *
   * @{
   */

      /**
	   * @ingroup Initialization
	   * @brief Initialisiert LCD, PWM und Ereigniswarteschlange.
	   *
	   * @details
       * - Das LCD-Modul wird über I2C mit Standardadresse konfiguriert und eingeschaltet.
       *   Anschließend wird das Display gelöscht, um mit einer sauberen Anzeige zu starten.
	   * - PWM-Signale auf drei Kanälen des Timers werden aktiviert.
   	   * - Drei Initialereignisse werden in die Ereigniswarteschlange eingefügt:
	   *   1. StartEvent: Startet das System.
	   *   2. TestEventLED: Testet eine LED mit einer bestimmten Farbe.
	   *   3. DisplayEvent: Zeigt eine Statusnachricht auf dem LCD an.
	   */
	  lcd.i2c = &hi2c2;
	  lcd.i2c_addr = LCD_DEFAULT_ADDR; // 0x27 << 1
	  lcd.backlight_enable = true;
	  LCD_Begin(&lcd);
	  LCD_Clear(&lcd);

	  HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);
	  HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_3);
	  HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_4);

	  event_queue.push(std::make_unique<StartEvent>());
	  event_queue.push(std::make_unique<TestEventLED>(htim4, GREEN));
	  event_queue.push(std::make_unique<DisplayEvent>(lcd, "Press the button",  "to start."));


	//!@}
  /**
   * @defgroup while-Schleife
   * @brief while-Schleife, welche durch ein interrupt unterbrochen wird
   *
   * @details
   * -Wenn die event_queue nicht leer ist, dann bewegt sich das Objekt nach vorne und wird
   * abgearbeitet (Also gepopt)
   * -Danach kommt es zu einer Verzögerung
   *@{
   */
  while(1)
  {
	if(!event_queue.empty())
	{
	  std::unique_ptr<Event> e = std::move(event_queue.front());

	  event_queue.pop();

	  e->handleEvent();
	}
  }

}
