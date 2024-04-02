#include "main.h"
#include <stdint.h>
#include <string.h>



#define is_dht_signal_low 			!HAL_GPIO_ReadPin(DHT11_port, DHT11_pin)
#define is_dht_signal_high			HAL_GPIO_ReadPin(DHT11_port, DHT11_pin)



void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM1_Init(void);
void blink_led(int state);
uint8_t init_DHT11(void);
void set_gpio_input(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
void set_gpio_output(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
int dht11_is_ready(void);
uint8_t read_dht11(void);
void delay_us(uint16_t us);



TIM_HandleTypeDef htim1;
int dht_high_signal_sent;
int dht_low_signal_sent;
uint8_t i,j;
uint8_t Rh_byte1, Rh_byte2, Temp_byte1, Temp_byte2;
float temp1, temp2, hum1, hum2;
uint16_t SUM;



int main(void){

  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();
  MX_TIM1_Init();
  HAL_TIM_Base_Start(&htim1);

  while (1) {
	  //unstable status 1seg delay
	  HAL_Delay(1000);
	  init_DHT11();
	  Rh_byte1 =  read_dht11();
	  hum1 = Rh_byte1;
	  Rh_byte2 =  read_dht11();
	  hum2 = Rh_byte2;
	  Temp_byte1 =  read_dht11();
	  temp1 = Temp_byte1;
	  Temp_byte2 =  read_dht11();
	  temp2 = Temp_byte2;
	  SUM = read_dht11();

//	  if (SUM == (Rh_byte1+Rh_byte2+Temp_byte1+Temp_byte2))
//	  	{
//		  HAL_Delay(1000);
//	  	}
//	  HAL_Delay(1000);
  }

}

void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
//  RCC_OscInitStruct.PLL.PLLM = 16;
//  RCC_OscInitStruct.PLL.PLLN = 336;
//  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 100;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  RCC_OscInitStruct.PLL.PLLR = 2;

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

//  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
//  {
//    Error_Handler();
//  }
  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
    {
      Error_Handler();
    }
}

static void MX_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();

  /*Configure GPIO pin : DHT11_pin */
  GPIO_InitStruct.Pin = GPIO_PIN_10;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}

static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 100-1;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 0xffff-1;
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

void blink_led(int state){

	GPIO_InitTypeDef GPIO_InitStruct = { 0 };
	GPIO_InitStruct.Pin = GPIO_PIN_12;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;

	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	if(state == 1){
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_12, 1);
	}else{
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_12, 0);
	}

}

uint8_t init_DHT11(void) {

	//if a low pulse followed by a high pulse, of 80us length both are sent after reset pulse
	//then the dht11 is ready to send data
	dht_high_signal_sent = 0;
	dht_low_signal_sent = 0;

	//reset pulse
	set_gpio_output(DHT11_port, DHT11_pin);
	HAL_GPIO_WritePin(DHT11_port, DHT11_pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(DHT11_port, DHT11_pin, GPIO_PIN_RESET);
	HAL_Delay(18);
	set_gpio_input(DHT11_port, DHT11_pin);

	//check dht11's response
	//maybe is not necesary to check if it is low
	while (is_dht_signal_low);
	dht_low_signal_sent = 1;

	while(is_dht_signal_high){
		//TODO crear como un reloj que si llegua a 400us determine que ha sufrido un timeout
	}
	dht_high_signal_sent = 1;

	return 1;
}

void set_gpio_input(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin) {

	GPIO_InitTypeDef GPIO_InitStruct = { 0 };
	GPIO_InitStruct.Pin = GPIO_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);

}

void set_gpio_output(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin) {

	GPIO_InitTypeDef GPIO_InitStruct = { 0 };
	GPIO_InitStruct.Pin = GPIO_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);

}

int dht11_is_ready(void){

	if(dht_high_signal_sent & dht_low_signal_sent){
		return 1;
	}

	return 0;

}

uint8_t read_dht11(void){

	if(!dht11_is_ready()){
		init_DHT11();
	}

	for (int j = 0; j < 8; ++j) {

		//start to transmit 1-bit data (50us)
		while (is_dht_signal_low);

		//26-28us voltage-lentgh bit means 0
		//70us voltage-lentgh bit means 1
		//so if it is low after 30us, it means that bit is 1
		delay_us(30);
		if (is_dht_signal_low) {
			i&= ~(1<<(7-j));	//bit is 0
		}
		else{
			i|= (1<<(7-j));		//bit is 1
		}
		while (is_dht_signal_high);

	}

	return i;

}

void delay_us(uint16_t us) {

	__HAL_TIM_SET_COUNTER(&htim1,0);  // set the counter value a 0
	while (__HAL_TIM_GET_COUNTER(&htim1) < us);  // wait for the counter to reach the us input in the parameter
}

void Error_Handler(void)
{
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
}

#ifdef  USE_FULL_ASSERT

void assert_failed(uint8_t *file, uint32_t line)
{
}
#endif
