/**  ******************************************************************************
 *
 * @file    main.c
 * @author  Ac6
 * @version V1.0
 * @date    29-March-2019
 * @brief   Default main function.
 ********************************************************************************/

#include "main.h"
#include "stm32l4xx.h"
#include "stm32l4xx_hal.h"
#include "stm32l475e_iot01.h"
#include "string.h"
#include "circularBuffer.h"
#include "stdbool.h"
#include "ECE631JSON.h"
#include "SerialWifiBridge.h"
#include "stm32l475e_iot01_accelero.h"

//Set up Receive Buffer
static commBuffer_t Rx;
//Set up Transmit Buffer
static commBuffer_t Tx;
//Gyro Value Array
int16_t pAccelDataXYZ[3] = {0};

int main(void){
	HAL_Init();
	BSP_ACCELERO_Init();
	/* Configure the System clock to have a frequency of 80 MHz */
	SystemClock_Config();
	BSP_LED_Init(LED2);
	BSP_LED_Off(LED2);
	/* Configure UART4 */
	initBuffer(&Rx, 0);
	initBuffer(&Tx, 1);
	Configure_USART();
	char* startText = "\n{\"Action\":\"Debug\",\"Info\":\"Testing UART4\"}\n";
	bool wifiSetup = false;
	bool mqttSetup = false;
	bool mqttSubbed = false;
	int statusCount = 2000;
	SendCharArrayUSART4(startText,strlen(startText));
	uint32_t PrevTicks = HAL_GetTick();
	/* Infinite loop */
	while(1){

		uint32_t CurrTicks = HAL_GetTick();

		if((CurrTicks - PrevTicks) >= statusCount){
			PrevTicks = CurrTicks;
			BSP_LED_Toggle(LED2);

			if(mqttSubbed == true){
				char measures[100] = "";
				char outBuff[300] = "";
				BSP_ACCELERO_AccGetXYZ(pAccelDataXYZ);
				long x_angle = (long)((180.0/2000.0)*(pAccelDataXYZ[0]+1000.0));
				long y_angle = (long)((180.0/2000.0)*(pAccelDataXYZ[1]+1000.0));
				pack_json("{s:n,s:n}", measures,"X", x_angle, "Y", y_angle);
				measures[strlen(measures) - 1] = '\0';
				pack_json("{s:{s:s,s:s},s:s}", outBuff, "MQTT","Topic","ece631/GyroMaze","Message",measures,"Action","MQTTPub");
				putMessage(&Tx, outBuff, strlen(outBuff));
				LL_USART_EnableIT_TXE(USARTx_INSTANCE);
			}
		}

		if(haveMessage(&Rx)){
			char tempBuff[MAXCOMMBUFFER] = "";
			char response[200] = "";//for GetNameValue
			char messageJSON[400] = "";//Used for the message section of any response json
			char wifiJSON[200] = "";//inside wifi message
			char pingMessage[50] = "";//inside ping inside wifi message
			char transBuff[MAXCOMMBUFFER] = "";//In WifiStatus and MQTTSetup for Action transmits
			char mqttJSON[100] = "";//In MQTTSetup
			char subscrJSON[50] = "";//In MQTTSubs
			getMessage(&Rx, tempBuff);
			GetNameValue(tempBuff, "Response", response);
			unsigned int state = BridgeResponseID(response);

			switch (state){
				case StartUp:
				break;
				case WifiStatus:
					GetNameValue(tempBuff, "Wifi", wifiJSON);
					GetNameValue(wifiJSON, "isGWPingable", pingMessage);
					if(strcmp(pingMessage, "null") == 0 && wifiSetup == false){
						wifiSetup = true;
						pack_json("{s:{s:s,s:s},s:s}", transBuff, "Wifi","SSID","ece631Lab","Password","stm32IOT!","Action","WifiSetup");
						putMessage(&Tx, transBuff, strlen(transBuff));
						LL_USART_EnableIT_TXE(USARTx_INSTANCE);
					}else if(strcmp(pingMessage, "true") == 0 && mqttSetup == false){
						statusCount = 5000;
						char transBuff[MAXCOMMBUFFER] = "";
						pack_json("{s:{s:s,s:s},s:s}", transBuff, "MQTT","Host","192.168.1.204","Port","1883","Action","MQTTSetup");
						putMessage(&Tx, transBuff, strlen(transBuff));
						LL_USART_EnableIT_TXE(USARTx_INSTANCE);
					}else{
					}
				break;
				case MQTTSetup:
					GetNameValue(tempBuff, "Message", messageJSON);
					GetNameValue(messageJSON, "MQTT", mqttJSON);
					if(strcmp(mqttJSON, "{\"Result\": \"Success Pub \"}") == 0){
						mqttSetup = true;
						statusCount = 1000;
						pack_json("{s:{s:[s]},s:s}", transBuff, "MQTT","Topics","ece631/GyroMaze","Action","MQTTSubs");
						putMessage(&Tx, transBuff, strlen(transBuff));
						LL_USART_EnableIT_TXE(USARTx_INSTANCE);
					}
				break;
				case MQTTSubs:
					GetNameValue(tempBuff, "Message", messageJSON);
					GetNameValue(messageJSON, "MQTT", mqttJSON);
					GetNameValue(mqttJSON, "Action", subscrJSON);
					if(strcmp(subscrJSON, "Subscribed") == 0){
						mqttSubbed = true;
						statusCount = 500;
						//BSP_GYRO_GetXYZ(pGyroDataXYZ);
						//pack_json("{s:{s:s,s:n},s:s}", transBuff, "MQTT","Topic","ece631/GyroMaze","Message",(long)pGyroDataXYZ[1],"Action","MQTTPub");
						//putMessage(&Tx, transBuff, strlen(transBuff));
						//LL_USART_EnableIT_TXE(USARTx_INSTANCE);
					}
				break;
				case MQTTPub:
				break;
				case SubscribedMessage:
					//GetNameValue(tempBuff, "Message", messageJSON);
					//GetNameValue(messageJSON, "MQTT", mqttJSON);
					//GetNameValue(mqttJSON, "Message", finalJSON);
					//pack_json("{s:{s:s,s:s},s:s}", transBuff, "MQTT","Topic","ece631/loopback","Message","Loop","Action","MQTTPub");
					//putMessage(&Tx, transBuff, strlen(transBuff));
					//LL_USART_EnableIT_TXE(USARTx_INSTANCE);
				break;
				default:
				break;
			}

		}
	}
}

/**
 * @brief  System Clock Configuration
 *         The system Clock is configured as follow :
 *            System Clock source            = PLL (MSI)
 *            SYSCLK(Hz)                     = 80000000
 *            HCLK(Hz)                       = 80000000
 *            AHB Prescaler                  = 1
 *            APB1 Prescaler                 = 1
 *            APB2 Prescaler                 = 1
 *            MSI Frequency(Hz)              = 4000000
 *            PLL_M                          = 1
 *            PLL_N                          = 40
 *            PLL_R                          = 2
 *            PLL_P                          = 7
 *            PLL_Q                          = 4
 *            Flash Latency(WS)              = 4
 * @param  None
 * @retval None  */

static void SystemClock_Config(void){
	RCC_ClkInitTypeDef RCC_ClkInitStruct;
	RCC_OscInitTypeDef RCC_OscInitStruct;
	/* MSI is enabled after System reset, activate PLL with MSI as source */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
	RCC_OscInitStruct.MSIState = RCC_MSI_ON;
	RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
	RCC_OscInitStruct.MSICalibrationValue = RCC_MSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
	RCC_OscInitStruct.PLL.PLLM = 1;
	RCC_OscInitStruct.PLL.PLLN = 40;
	RCC_OscInitStruct.PLL.PLLR = 2;
	RCC_OscInitStruct.PLL.PLLP = 7;
	RCC_OscInitStruct.PLL.PLLQ = 4;

	if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		/* Initialization Error */
		while(1);
	}  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2
	clocks dividers */

	RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
	{    /* Initialization Error */
		while(1);
	}
}
/*
 * This is a simple IRQ handler for USART4. A better design would
 * be to have a circular list of strings. Head index pointing to
 * where chars added. Tail index oldest string to be processed
 */

void USARTx_IRQHandler(void)
{
	// check if the USART6 receive interrupt flag was set
	if(LL_USART_IsActiveFlag_RXNE(USARTx_INSTANCE)){
		char t = LL_USART_ReceiveData8(USARTx_INSTANCE);
		putChar(&Rx, t);
	}
		/* check if the received character is not the LF character (used to determine end of string)
		 * or the if the maximum string length has been been reached */
	if(LL_USART_IsActiveFlag_TXE(USARTx_INSTANCE)){
		if (haveMessage(&Tx)){
			char t = getChar(&Tx);
			LL_USART_TransmitData8(USARTx_INSTANCE, t);
		}else{
			// otherwise reset the character counter and print the received string
			LL_USART_DisableIT_TXE(USARTx_INSTANCE);
		}
	}
	USARTCount++;
}
