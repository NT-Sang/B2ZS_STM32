/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body - B2ZS Encoding with MCP4725 DAC Output
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#include "stdio.h"
#include <stdarg.h>
#include <string.h>

#define Address_device 0x60<<1


/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

TIM_HandleTypeDef htim1;

UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */
void my_printf (const char* format, ... )
{
    va_list args;
    va_start (args, format);

    char buffer[256];
    vsnprintf (buffer, sizeof (buffer), format, args);

    // Gui du lieu qua UART
    HAL_UART_Transmit (&huart1, (uint8_t*) buffer, strlen (buffer), HAL_MAX_DELAY) ;

    va_end (args);
}


uint8_t input[40];
uint8_t dodai_data[16]={0};
uint8_t header[8] = {1,1,1,1,1,1,1,1};

int8_t output[sizeof(input) + sizeof(dodai_data)+sizeof(header)];
int8_t b2zs_ouput[sizeof(input) + sizeof(dodai_data)+sizeof(header)]={0};

uint8_t data[2];
uint8_t data_index =0;
uint8_t counter =0;

volatile uint8_t i2c_tx_ready = 1;  

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_TIM1_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

void dodai(int size)
{

  uint8_t sodu =0;
  uint8_t i =0;
  while(size!=0){
      sodu = size%2;
      size=size/2;
      dodai_data[sizeof(dodai_data)-i-1]=sodu;
      i++;
  }
}

		void B2ZS_coding(uint8_t input[], uint8_t size) {
			dodai(size);
			
	
			for(int i= 0; i<sizeof(header);i++)
            {
                output[i] = header[i];
            }
            for(int i= 0; i<sizeof(dodai_data);i++)
            {
                output[i+sizeof(header)] = dodai_data[i];
            }
            for(int i= 0; i<size;i++)
            {
                output[i+sizeof(header)+ sizeof(dodai_data)] = input[i];
            }
			
			
			int8_t last_polarity = -1; 

			for (uint8_t i = 0; i < sizeof(output); i++) {
					if (output[i] == 1) {
							last_polarity = -last_polarity;  
							b2zs_ouput[i] = last_polarity;
						
					}
					else if (i < sizeof(output) - 1 && output[i] == 0 && output[i + 1] == 0) {
							last_polarity = -last_polarity;  
							b2zs_ouput[i] = last_polarity;
											
							b2zs_ouput[i + 1] = last_polarity; 
		
							i++;
							
					}
					else {
              b2zs_ouput[i] = 0;
											
					}
			}
		}
		
		
void Set_Value(int8_t b2zs_ouput, I2C_HandleTypeDef *hi2c) 
{

				uint16_t dac_value;
        if (b2zs_ouput == -1) 
					{
            dac_value = 0x0000;
					} 
				else if (b2zs_ouput == 0)
					{
            dac_value = 0x0800;  
					} 
				else
					{
            dac_value = 0x0FFF;   
					}
				 // Debug giá tri DAC
//        char msg[30];
//        sprintf(msg, "DAC Value: %04X\r\n", dac_value);
//        HAL_UART_Transmit(&huart1, (uint8_t *)msg, strlen(msg), HAL_MAX_DELAY);
				
				data[0] = (dac_value >> 8) & 0xFF;
				data[1] =  dac_value  & 0xFF;
			  
				 if (i2c_tx_ready) {
        i2c_tx_ready = 0;
        if (HAL_I2C_Master_Transmit_IT(&hi2c1, Address_device, data, 2) != HAL_OK) {
            Error_Handler();
        }
    }
		
}

    
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
		{
			if(huart->Instance == huart1.Instance)
			{		
				for(uint8_t i=0;i<sizeof(input);i++)
				{
					if(input[i]=='1')
					{
						input[i]=1;
					}
					else
					{
						input[i]=0;
					}
				}
				
			
					B2ZS_coding(input, sizeof(input));
				
//				for(int i=0; i<sizeof(output);i++)
//    {
//     my_printf("%d ",(int)output[i]);
//    }
//    my_printf("\n");
//		for(int i=0; i<sizeof(b2zs_ouput);i++)
//    {
//     my_printf("%d ",(int)b2zs_ouput[i]);
//    }
//		my_printf("\n");
		
		
		
					HAL_TIM_Base_Start_IT(&htim1);
				

				
						
			}
		}
		

		

		
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{ 
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_SET);
	if(htim->Instance==htim1.Instance)
	{
		if(counter <40)
		{
			
				Set_Value(b2zs_ouput[data_index], &hi2c1);
			  counter++;
			
		}
		else if(counter >=40 && counter <80)
		{ 
			int8_t output8 = 0;
			Set_Value(output8, &hi2c1);
			counter ++;
		}
		else
		{
			// gui bit tiep theo
			counter =0;
			if(data_index<=sizeof(input) + sizeof(dodai_data)+sizeof(header)-2)
			{
				data_index++;
				Set_Value(b2zs_ouput[data_index], &hi2c1);
			  counter++;
			}
			else
			{
					data_index =0;
				HAL_TIM_Base_Stop(&htim1);
				HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_RESET);
					HAL_UART_Receive_IT(&huart1, input, sizeof(input));
			}
			
			
		}
		
		
	}
}
void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *hi2c)
{
    if (hi2c->Instance == hi2c1.Instance) {
        i2c_tx_ready = 1;
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
  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_USART1_UART_Init();
  MX_TIM1_Init();
  /* USER CODE BEGIN 2 */
	HAL_UART_Receive_IT(&huart1, input, sizeof(input));
	
  
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

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

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 71;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 124;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
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
