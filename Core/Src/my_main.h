#include "main.h"

#ifdef __cplusplus
extern "C"
{
  #endif

  /**
   * @brief Interrupt fürs Blinken
   * @param htim
   */
  void HAL_TIM_PeriodElapsedCallback_(TIM_HandleTypeDef *htim);

  /**
   * @brief Interrupt, welcher den Operierenden Betrieb enthält
   * @param GPIO_Pin
   */
  void HAL_GPIO_EXTI_Callback_(uint16_t GPIO_Pin);

  /**
   * @brief  The application entry point.
   */
  void my_main();

  #ifdef __cplusplus
}
#endif
