/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart2;
uint32_t last_press[3] = {0}; 	// Array que almacena el tiempo de los botones de la luz izquierda y la derecha
bool leftLightBlinking = false;		// Variable que indica si oprimieron dos veces o no la direccional izquierda
bool rightLightBlinking = false;	// Variable que indica si oprimieron dos veces o no la direccional derecha
uint32_t last_double_press = 0;		// Último registro de doble pulsación

uint32_t counter_left = 0; //contadores de luz izquierda para el número de blinkings
uint32_t counter_right = 0;//contadores de luz derecha para el numero de blinkings


uint8_t index_pines = 0; //variable global para el control de los botones presionados

bool status_stationary = false;
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(GPIO_Pin);

  uint32_t current_time = HAL_GetTick();//variable para obtener el tiempo actual de procesamiento
  uint8_t index_btn;//variable para el control del botón presionado
  uint32_t current_double_time; //variable de control para las dobles pulsaciones

  // Para identificar el botón presionado:
  if(GPIO_Pin == BUTTON_LEFT){
	  index_btn = 1;
	  index_pines = 1;
	  HAL_UART_Transmit(&huart2, "Luz izquierda\r\n",15,10);
	 counter_left = 6;
  }else if(GPIO_Pin == BUTTON_RIGHT){
	  index_btn = 2;
	  index_pines = 2;
	  HAL_UART_Transmit(&huart2, "Luz derecha\r\n",13,10);
	  counter_right = 6;
  }

  //lógica para las dobles pulsaciones
  if(current_time - last_press[index_btn] > 200){ //se verifica si la segunda pulsación se dio en menos del tiempo dado
 	  last_press[index_btn] = current_time; // Actualiza el tiempo
 	 if((index_btn == 1 || index_btn == 2)){ //se verifica si alguno de los botones fue presionado
 		current_double_time = HAL_GetTick();

 		if (current_double_time - last_double_press <= 300) {
			  // Si se realizó la doble pulsación :
			  if (index_btn == 0) {
				  leftLightBlinking = true;		// Encender indefinidamente la luz izquierda
				  rightLightBlinking = false;	// Apagar la derecha si estaba encendida
				  HAL_UART_Transmit(&huart2, "Doble Izquierda\r\n", 15,10);
			  } else {
				  rightLightBlinking = true;	// Encender indefinidamente la luz derecha
				  leftLightBlinking = false;	// Apagar la izquierda si estaba encendida
				  HAL_UART_Transmit(&huart2, "Doble Derecha\r\n", 13,10);
		  }
 		}
 	 }// Tiempo de las dobles pulsaciones
  }
}

void turnsignalleft(void){ //función para el parpadeo de la luz izquierda
  	static uint32_t turntoggle_tick = 0;
  	if (turntoggle_tick < HAL_GetTick() && counter_left > 0){
  		turntoggle_tick = HAL_GetTick()+500;
  		HAL_GPIO_TogglePin(LED_LEFT_GPIO_Port, LED_LEFT_Pin);
  		counter_left--;
  	}
  }

  void turnsignalright(void){ //función parpadeo de la luz derecha
  	static uint32_t turntoggle_tick = 0;
  	if (turntoggle_tick < HAL_GetTick() && counter_right > 0){
  		turntoggle_tick = HAL_GetTick()+500;
  		HAL_GPIO_TogglePin(LED_RIGHT_GPIO_Port, LED_RIGHT_Pin);
  		counter_right--;
  	}
  }
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */
	  if(index_pines == 1){
	  	turnsignalleft();

	  	 }

	   if(index_pines == 2){
	  	  turnsignalright();

	   }
    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
