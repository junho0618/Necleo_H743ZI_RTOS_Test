/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "usart.h"

#include "data.h"
#include "debug.h"
#include "ringbuff.h"

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
/* USER CODE BEGIN Variables */
properties_t struPropertie;

/* USER CODE END Variables */
osThreadId defaultTaskHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
U8 initVariable( void );
/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void const * argument);

extern void MX_USB_DEVICE_Init(void);
void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */
	U8	ret = 0;

	ret = initVariable();
	if( ret )
	{
		jeprintf( "Fail initialize Variable!!!\r\n" );
		while( 1 );
	}

	InitRingBuffer( &rbUartRx );
	HAL_UART_Receive_IT( &huart2, &rbUartRx.dummy, 1 );
  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 128);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void const * argument)
{
  /* init code for USB_DEVICE */
  MX_USB_DEVICE_Init();

  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
	for(;;)
	{
		HAL_GPIO_TogglePin( LED1_GPIO_Port, LED1_Pin );
		osDelay(1000);
	}
  /* USER CODE END StartDefaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
U8 initVariable( void )
{
	U8 ret = INIT_OK;

	jiprintf( "%s\r\n", __func__ );
	//-----------------------------------------------------------------
	//   Status Check Flag
	//-----------------------------------------------------------------
	struPropertie.mUartInFlag	= 0;

	//-----------------------------------------------------------------
	//   Memory Pool variable
	//-----------------------------------------------------------------
	osPoolDef( uartcommpool, UART_POOL_SIZE, UartPkt_t );
	struPropertie.hUartCommPool = osPoolCreate( osPool( uartcommpool ) );
	if( struPropertie.hUartCommPool == NULL )
	{
		jeprintf( "error... osPoolCreate uartcommpool\r\n" );
		ret = INIT_FAIL;
	}

	//-----------------------------------------------------------------
	//   Message variable
	//-----------------------------------------------------------------
	osMessageQDef( incommmessage, MESSAGE_QUEUE_SIZE, MsgClst_t );
	struPropertie.hInCommMessage = osMessageCreate( osMessageQ( incommmessage ), NULL );
	if( struPropertie.hInCommMessage == NULL )
	{
		jeprintf( "error... osMessageCreate incommmessage\r\n" );
		ret = INIT_FAIL;
	}

	//-----------------------------------------------------------------
	//   Thread variable
	//-----------------------------------------------------------------
	osThreadDef( receive, receiveUartThread, osPriorityNormal, 0, 128 );
    struPropertie.hReceiveUartThread = osThreadCreate( osThread( receive ), NULL );

    osThreadDef( send, sendDataThread, osPriorityNormal, 0, 128 );
    struPropertie.hSendDataThread = osThreadCreate( osThread( send ), NULL );

	return ret;
}

/*-------------------------------------------------------------------
	UART Protocol
---------------------------------------------------------------------
	Header : |G|I|T|lenH|lenL|				Total 5Byte
	Data									Total lenH * 256 + lenL
	CheckSum								Total 1Byte
-------------------------------------------------------------------*/
void receiveUartThread( void const *argument )
{
	UartPkt_t	*packet;
	MsgClst_t	message;

	U32			len;
	U32			i;
	U32			waitTime;

	U8			checksum = 0;
	U8			data;

	for(;;)
	{
		HAL_GPIO_TogglePin( LED2_GPIO_Port, LED2_Pin );
		osDelay( 100 );

		/* Uart	Start(GIT)가 들어올때까지 Wait */
		if( GetQueueSize( &rbUartRx ) < UART_PROTOCOL_START_SIZE )
		{
			continue;
		}
		else
		{
			Dequeue( &rbUartRx, &data );
			if( data == 'G'	)
			{
				Dequeue( &rbUartRx, &data );
				if(	data == 'I' )
				{
					Dequeue( &rbUartRx, &data );
					if(	data == 'T' )
					{
						/* Data Length(2Byte)가 들어 올때 까지 대기 */
						while( GetQueueSize( &rbUartRx ) < 2 )
						{
							osDelay( 10 );
						}

						/* Get Data	Length */
						Dequeue( &rbUartRx, &data );
						len	= data * 256;
						Dequeue( &rbUartRx, &data );
						len	= len +	data;
						jprintf( "data len = %d\r\n", len );

						/* 데이터 및 CheckSum이 수신될때까지 대기 */
						waitTime = 300;		// 3s
						while( GetQueueSize( &rbUartRx ) < len + 1 )
						{
							waitTime--;
							if(	waitTime ==	0 )
							{
								jeprintf(	"error... timeout receive uart data!!\r\n" );
								while( !IsEmpty( &rbUartRx ) )
								{
									Dequeue( &rbUartRx, &data );
								}

								break;
							}

							osDelay( 10	);
						}

						if(	waitTime ==	0 )		continue;						// uart	Reset

						/* 수신된 데이터를 Packet에	저장 */
						packet			= ( UartPkt_t* )osPoolAlloc( struPropertie.hUartCommPool );
						packet->mLen	= len;						
						
						for( i = 0; i < len; i++ )
						{
							Dequeue( &rbUartRx, &packet->mData[i] );
							jprintf( "0x%02x ", packet->mData[i] );

							checksum += packet->mData[i];
						}
						
						jnprintf();

						/* 수신된 데이터 CheckSum Check */
						Dequeue( &rbUartRx, &data );
						if( checksum != data )
						{
							jeprintf( "Fail CheckSum(receive:0x%02x, calculate:0x%02x)...\r\n", data, checksum );
							continue;
						}
						
						checksum = 0;

						/* Packet을 InCommMessage에 전달 */
						message.mCommSource	= UART_INTERFACE;
						message.pPacket		= (void*)packet;
						osMessagePut( struPropertie.hInCommMessage, (uint32_t)&message, osWaitForever );
					}
				}
			}
		}
	}
}

void sendDataThread( void const *argument )
{
	osEvent		evt;
	MsgClst_t	*message;

	for(;;)
	{
		evt		= osMessageGet( struPropertie.hInCommMessage, osWaitForever );
		message = ( MsgClst_t * )evt.value.p;

		switch( message->mCommSource )
		{
			case USB_INTERFACE	:
				break;

			case UART_INTERFACE	:
			{
				UartPkt_t	*packet;
				U32			i;

				packet	= (UartPkt_t*)message->pPacket;
				for( i = 0; i < packet->mLen; i++ )
				{
					jprintf( "0x%02x ", packet->mData[i] );
				}

				jnprintf();

				break;
			}

			case WIFI_INTERFACE	:
				break;

			case BT_INTERFACE	:
				break;

			default				:
				break;
		}

		HAL_GPIO_TogglePin( LED3_GPIO_Port, LED3_Pin );
		osDelay(100);
	}
}

/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/