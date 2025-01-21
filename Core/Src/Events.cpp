/**
 * @file Events.cpp
 * @brief Events.cpp enthält Evente für zb. das Display
 */

#include "Events.h"
#include <string>
#include <cmath>
#include <vector>
#include <iomanip>
extern "C"
{
  #include "lcd/lcd.h"
}

const double ADC_MAX_VALUE = 4095;
const double ADC_MIN_VALUE = 0;

extern TIM_HandleTypeDef htim3;

extern volatile State currentState;
extern volatile uint8_t buttonPressed;
extern double timeConstant;
extern double timeConstantCalc;
extern double deviation ;
volatile double result1;
volatile double result2;
volatile double result3;
volatile double averageResults1 = 0;
volatile double averageResults2 = 0;
volatile double averageResults3 = 0;
volatile double sample = 0;
extern volatile double capacity;
extern volatile double resistance;
volatile double currentVolt = 0;
volatile double counter = 0;
extern uint16_t numberMeasurements; // each measurements consists of discharging and charging measurements
extern double potUsed;


void Event::handleEvent(){}
Event::~Event(){}

/**
 * @brief Startevent, bei welchem am Anfang ein IDLE-state ausgeführt wird.
 */
void StartEvent::handleEvent()
{
  currentState = IDLE;
  buttonPressed = 0;
  timeConstant = 0;
  result1 = 0;
  result2 = 0;
  result3 = 0;
  averageResults1 = 0;
  averageResults2 = 0;
  averageResults3 = 0;
  sample = 0;
  capacity = 10;
  currentVolt = 0;
  counter = 0;
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
  buttonPressed = 0;
  timeConstant = 0;
  result1 = 0;
  result2 = 0;
  result3 = 0;
  averageResults1 = 0;
  averageResults2 = 0;
  averageResults3 = 0;
  sample = 0;
  capacity = 10;
  currentVolt = 0;
  counter = 0;
}

CancelEvent::~CancelEvent() {}


DisplayEvent::DisplayEvent(LCD_HandleTypeDef& lcd, std::string text, int cutPosition): p_lLcd(lcd)
, p_sText(text), p_iCutPosition(cutPosition){}

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
  if (currentState == SHOW_RESULTS)
  {
	std::string texts[6];
	std::ostringstream oss;
	oss << std::fixed << std::setprecision(5);
	oss << timeConstant;
	texts[0] = "Measured time ";
	texts[1] = "constant: " + oss.str() + "s";
	oss.str("");
	oss << timeConstantCalc;
	texts[2] = "Calculated time ";
	texts[3] = "constant: " + oss.str() + "s";
	oss.str("");
	oss << deviation * 100;
	texts[4] = "Deviation: ";
	texts[5] = oss.str() + " pct";
	oss.str("");

	for (int i = 0; i < 3; i++)
	{
	  LCD_Clear(&p_lLcd);
	  LCD_SetCursor(&p_lLcd, 0, 0);
	  LCD_Printf(&p_lLcd, texts[2*i].c_str());
	  LCD_SetCursor(&p_lLcd, 1, 0);
	  LCD_Printf(&p_lLcd, texts[2*i+1].c_str());
	  buttonPressed = 0;
	  for (int i = 0; i < 4000/2; i++)
	  {
		if (buttonPressed){return;}
		HAL_Delay(1);
	  }
	}
  }
  else
  {
	if (p_sText.length() <= 16)
	{
	  text1 = p_sText;
	}
	else
	{
	  while (p_iCutPosition > 0 && p_sText[p_iCutPosition] != ' ')
	  {
	    --p_iCutPosition;
	  }
	  text1 = p_sText.substr(0, p_iCutPosition);
	  text2 = p_sText.substr(p_iCutPosition + 1);
	}
	LCD_Clear(&p_lLcd);
	LCD_SetCursor(&p_lLcd, 0, 0);
	LCD_Printf(&p_lLcd, text1.c_str());
	LCD_SetCursor(&p_lLcd, 1, 0);
	LCD_Printf(&p_lLcd, text2.c_str());
  }



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

//extern LCD_HandleTypeDef lcd;
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

// result1 = the average of the samples. Each sample is the result of the calculation with ln for every
// time value obtained during the while-loop
// result2 = the time it takes for the capacitor to be charged 63.2%
// result3 = the one fifth of the total time it takes for the capacitor to be fully charged
MeasurementEvent::MeasurementEvent(TIM_HandleTypeDef& htim, ADC_HandleTypeDef& hadc): p_tHtim(htim), p_aHadc(hadc) {}
void MeasurementEvent::handleEvent()
{
  double maxVolt = 3.25; // !! change 40
  double minVolt = 0.0; // !! change 40

  volatile double argument = 0; // argument for the ln calculation for result1
  const double epsilon = 1e-6;
  //  volatile double sample = 0;
  std::vector<double> samples; // a vector of the samples for result1
  volatile double sumSamples = 0; // sum of the samples for result1
  volatile double averageSamples = 0; // average of the samples for result1
  volatile uint8_t timerSampled = 0; // to control if the timer's been sampled at V(t) = 63.2% for result2

  double results[3][numberMeasurements*2]; // a 2D array to save every result from each iteration
  double sumResults[3]; // sum of the averages of each result
  double averageResults[3];
  double average = 0; // average of the results

  for (int i = 0; i < numberMeasurements; i++)
  {
	// reset the counter at the start of each measurement (before charging measurements)
	__HAL_TIM_SET_COUNTER(&p_tHtim, 0);
	timerSampled = 0; // (result2)
	// start the timer
	HAL_TIM_Base_Start(&p_tHtim);
	// turn on the voltage source
	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_13, GPIO_PIN_SET);
	// charge the capacitor fully
	while(currentVolt < maxVolt-0.036)
	{
	  counter = (double)__HAL_TIM_GET_COUNTER(&p_tHtim) / 1000; // in s
	  HAL_ADC_Start(&p_aHadc);
	  if (HAL_ADC_PollForConversion(&p_aHadc, HAL_MAX_DELAY) == HAL_OK)
	  {
	    currentVolt = 3.3 * (HAL_ADC_GetValue(&p_aHadc) / ADC_MAX_VALUE);
	  }
	  HAL_ADC_Stop(&p_aHadc);

	  // calculate the time constant from the formula and add it to the vector (result1)
	  argument = 1.0 - currentVolt / maxVolt;
	  if (argument > epsilon)
	  {
	    sample = (-1.0)* counter / log(argument);
		samples.push_back(const_cast<double&>(sample));
	  }

	  // sample the time when the capacitor is 63.2% charged and add it to the array (result2)
	  if ((currentVolt > maxVolt*(63.2*0.01)) & (timerSampled == 0))
	  {
		timerSampled = 1; // so that it won't get sampled again once it is sampled
		result2 = (double)__HAL_TIM_GET_COUNTER(&p_tHtim) / 1000;
		results[1][2*i] = const_cast<double&>(result2);
	  }

	  // wait for 20 ms between iterations for more accuracy
	  for (int i = 0; i < 20; i++)
	  {
		if (buttonPressed){return;}
		HAL_Delay(1);
	  }
	}

	// stop the timer when charging is completed
	HAL_TIM_Base_Stop(&p_tHtim);
	// find the average of the samples from the measurements and add it to the array (result1)
	for (double samp: samples)
	{
	  sumSamples += samp;
	}
	averageSamples = sumSamples/(samples.size());
	result1 = averageSamples;
	if (!std::isnan(result1))
	{
	  results[0][2*i] = const_cast<double&>(result1);
	}
	else
	{
	  results[0][2*i] = const_cast<double&>(result2);
	}
	// clear the vector and reset the sum for the next measurement (result1)
	samples.clear();
	sumSamples = 0;

	// calculate 1/5 of the total time and add it to the array (result3)
	result3 = ((double)__HAL_TIM_GET_COUNTER(&p_tHtim)/5) / 1000;
	results[2][2*i] = const_cast<double&>(result3);


	// reset the counter at the start of each measurement (before discharging measurements)
	__HAL_TIM_SET_COUNTER(&p_tHtim, 0);
	timerSampled = 0;

	// start the timer (to be stopped when the capacitor is 36.8% charged (63.2% discharged))
	HAL_TIM_Base_Start(&p_tHtim);

	// turn off the voltage source
	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_13, GPIO_PIN_RESET);
	// discharge the capacitor fully
	while(currentVolt > minVolt+0.036)
	{
	  counter = (double)__HAL_TIM_GET_COUNTER(&p_tHtim) / 1000; // in s
	  HAL_ADC_Start(&p_aHadc);
	  if (HAL_ADC_PollForConversion(&p_aHadc, HAL_MAX_DELAY) == HAL_OK)
	  {
		  currentVolt = 3.3 * (HAL_ADC_GetValue(&p_aHadc) / ADC_MAX_VALUE);
	  }
	  HAL_ADC_Stop(&p_aHadc);


	  argument = currentVolt / maxVolt;
	  if (argument > epsilon)
	  {
		sample = (-1.0)* counter / log(argument);
		samples.push_back(const_cast<double&>(sample));
	  }


	  // sample the time when the capacitor is 36.8% charged (63.2% discharged)
	  if ((currentVolt < maxVolt*(36.8*0.01)) & (timerSampled == 0))
	  {
		timerSampled = 1;
		// the measured time constant for discharging
		result2 = (double)__HAL_TIM_GET_COUNTER(&p_tHtim) / 1000;
		// add the found result to the vector
		results[1][2*i+1] = const_cast<double&>(result2);
	  }

	  // wait for 20 ms between iterations for more accuracy
	  for (int i = 0; i < 20; i++)
	  {
		if (buttonPressed){return;}
		HAL_Delay(1);
	  }
	}

	// stop the timer when discharging is completed
	HAL_TIM_Base_Stop(&p_tHtim);

	// add up the results from the measurements
	for (double samp: samples)
	{
	  sumSamples += samp;
	}
	// calculate the average
	averageSamples = sumSamples/(samples.size());
	result1 = averageSamples;
	if (!std::isnan(result1))
	{
	  results[0][2*i+1] = const_cast<double&>(result1);
	}
	else
	{
	  results[0][2*i+1] = const_cast<double&>(result2);
	}
	samples.clear();
	sumSamples = 0;

	result3 = ((double)__HAL_TIM_GET_COUNTER(&p_tHtim)/5) / 1000;
	results[2][2*i+1] = const_cast<double&>(result3);

  }

  for (int i = 0; i < 3; i++)
  {
	for (int j = 0; j < numberMeasurements*2; j++)
	{
		sumResults[i] += results[i][j];
	}
	averageResults[i] = sumResults[i]/(numberMeasurements*2);
  }

  averageResults1 = averageResults[0];
  averageResults2 = averageResults[1];
  averageResults3 = averageResults[2];

  if (potUsed)
  {
    average = (averageResults[0] + averageResults[1] + averageResults[2]) / 3;
  }
  else
  {
	double diff = 0;
	double minDiff = std::abs(timeConstantCalc - averageResults[0]);
	average = averageResults[0];
	for (int i = 1; i < 3; i++)
	{
	  diff = std::abs(timeConstantCalc - averageResults[i]);
	  if (diff < minDiff)
	  {
		average = averageResults[i];
		minDiff = diff;
	  }
	}
  }
  // the result is equal to the time constant
  timeConstant = average;
}

MeasurementEvent::~MeasurementEvent(){}

// 1 measurement takes 10.tau
// 40s = maximum time for the operation (if 1 measurement takes less than 40)
void CalculationEvent::handleEvent()
{
  timeConstantCalc = resistance * capacity / 1000; // in s
  numberMeasurements = 4 / timeConstantCalc;
  if (numberMeasurements < 1)
  {
	numberMeasurements = 1;
  }
  else if (numberMeasurements > 10)
  {
	  numberMeasurements = 10;
  }

}
CalculationEvent::~CalculationEvent() {}


void ComparisonEvent::handleEvent()
{
  deviation = (timeConstant - timeConstantCalc) / timeConstantCalc;
}

ComparisonEvent::~ComparisonEvent(){}
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
