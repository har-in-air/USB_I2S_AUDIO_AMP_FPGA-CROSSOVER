#include "main.h"
#include "spi.h"
#include "i2c.h"
#include "usart.h"
#include "biquad.h"
#include "tas5753md.h"

USBD_HandleTypeDef USBD_Device;
AUDIO_STATUS_TypeDef audio_status;
static int fs = 0;

void SystemClock_Config(void);


int main(void){
  HAL_Init();
  SystemClock_Config();
  spi_init();
  MX_I2C1_Init();

  MX_USART2_UART_Init();
  printMsg("\r\nUSB Audio I2S Bridge\r\n");

  bsp_init();
  HAL_Delay(200); // allow some time for FPGA bitstream to load
  biquad_loadCoeffs_LR(48000.0);
  tas5753md_config();

  // Init Device Library
  USBD_Init(&USBD_Device, &AUDIO_Desc, 0);
  // Add Supported Class
  USBD_RegisterClass(&USBD_Device, USBD_AUDIO_CLASS);
  // Add Interface callbacks for AUDIO Class
  USBD_AUDIO_RegisterInterface(&USBD_Device, &USBD_AUDIO_fops);
  // Start Device Process
  USBD_Start(&USBD_Device);
  
  while (1) {
 	if (audio_status.frequency != fs) {
 		printMsg("Fs = %d\r\n",audio_status.frequency);
 	    fs = audio_status.frequency;
   	  }
     switch (audio_status.frequency) {
       case 44100:
         BSP_LED_On(LED_GREEN);
         BSP_LED_Off(LED_BLUE);
         break;
       case 48000:
         BSP_LED_Off(LED_GREEN);
         BSP_LED_On(LED_BLUE);
         break;
       default:
         BSP_LED_Off(LED_GREEN);
         BSP_LED_Off(LED_BLUE);
     }

     HAL_Delay(100);
 #ifdef DEBUG_FEEDBACK_ENDPOINT // see Makefile C_DEFS
     // see USBD_AUDIO_SOF() in usbd_audio.c
 	if (BtnPressed) {
 		BtnPressed = 0;
 		printMsg("DbgOptimalWritableSamples = %d\r\nDbgSafeZoneWritableSamples = %d\r\n", AUDIO_TOTAL_BUF_SIZE/(2*6), AUDIO_BUF_SAFEZONE_SAMPLES);
 		printMsg("DbgMaxWritableSamples = %d\r\nDbgMinWritableSamples = %d\r\n\r\n", DbgMaxWritableSamples, DbgMinWritableSamples);
 		int count = 256;
 		while (count--){
 			// print oldest to newest
 			printMsg("%d %d %f\r\n", DbgSofHistory[DbgIndex], DbgWritableSampleHistory[DbgIndex], DbgFeedbackHistory[DbgIndex]);
 			DbgIndex++;
 			}
 		}
 #endif

   }
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  /** Configure the main internal regulator output voltage 
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_I2S;
  PeriphClkInitStruct.PLLI2S.PLLI2SN = 424;
  PeriphClkInitStruct.PLLI2S.PLLI2SR = 3;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
}


void printMsg(char* format, ...) {
	char sz[100];
	va_list args;
	va_start(args, format);
	vsprintf(sz, format, args);
	HAL_UART_Transmit(&huart2, (uint8_t *)sz, strlen(sz), HAL_MAX_DELAY);
	va_end(args);
	}


void Error_Handler(void){
	uint32_t counter;
	while(1){
		BSP_OnboardLED_Toggle();
		counter = 0xFFFF;
		while (counter--) ;
		}
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
    printMsg("Wrong parameters value: file %s on line %d\r\n", file, line);
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
