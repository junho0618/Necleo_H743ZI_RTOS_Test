/*----------------------------------------------------------------------
 *   RTOS platform data & defines
 *--------------------------------------------------------------------*/
#ifndef __DATA_H__
#define __DATA_H__
 
/*----------------------------------------------------------------------
 *   Include
 *--------------------------------------------------------------------*/
#include "stm32h7xx.h"
#include "cmsis_os.h"
#include "string.h"
#include "typedef.h"
#include "ringbuff.h"

/*----------------------------------------------------------------------
 *   Defines
 *--------------------------------------------------------------------*/
#define	UART_POOL_SIZE								10
#define	MESSAGE_QUEUE_SIZE							10

#define	PROTOCOL_MIN_SIZE							3
#define	UART_PACKET_SIZE							512
#define	DIAGNOSTIC_PACKET_SIZE						100
#define	OUTCOM_PACKET_SIZE							2048

#define	UART_PROTOCOL_START_SIZE					3


/*----------------------------------------------------------------------
 *   typedef
 *--------------------------------------------------------------------*/
typedef enum
{
	USB_INTERFACE	= 0 ,
	UART_INTERFACE	= 1 ,
	WIFI_INTERFACE	= 2 ,
	BT_INTERFACE	= 3
} COMM_Interface;

typedef enum
{
	CAN_INTERFACE	= 0 ,
	KWP_INTERFACE	= 1 ,
	ETH_INTERFACE	= 2
} DIAG_Interface;

/*----------------------------------------------------------------------
 *   Structure
 *--------------------------------------------------------------------*/
typedef struct
{
	uint16_t	mLen;
	uint8_t		mData[UART_PACKET_SIZE];
} UartPkt_t;

typedef struct
{
	uint8_t		mCommSource;	
	void		*pPacket;
} MsgClst_t;

typedef struct
{
	//-----------------------------------------------------------------
	//   Status Check Flag
	//-----------------------------------------------------------------
	uint8_t			mUartInFlag;

	//-----------------------------------------------------------------
	//   Memory Pool variable
	//-----------------------------------------------------------------
	osPoolId		hUartCommPool;

	//-----------------------------------------------------------------
	//   Message variable
	//-----------------------------------------------------------------
	osMessageQId	hInCommMessage;
	
	//-----------------------------------------------------------------
	//   Thread variable
	//-----------------------------------------------------------------
	osThreadId		hReceiveUartThread;
	osThreadId		hSendDataThread;
} properties_t;

/*----------------------------------------------------------------------
 *   Functions
 *--------------------------------------------------------------------*/

void receiveUartThread( void const *argument );
void sendDataThread( void const *argument );

extern properties_t p_properties;

#endif // __DATA_H__