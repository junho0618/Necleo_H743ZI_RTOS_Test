#ifndef __RINGBUFF_H__
#define __RINGBUFF_H__
 
#include "stm32h7xx_hal.h"
#include "typedef.h"

/*---------------------------------------------------------------------------------------------
     Defines
---------------------------------------------------------------------------------------------*/
#define	QUEUE_SIZE					512
#define	NEXT(index)					( ( index + 1 ) % QUEUE_SIZE )

/*---------------------------------------------------------------------------------------------
	Typedefs
---------------------------------------------------------------------------------------------*/
typedef struct
{
	U8		buf[QUEUE_SIZE];	
	U8		dummy;	
	U32		count;
	VU32	rxd;
	VU32	front;
	VU32	rear;
} RingBuffer_t;

typedef enum
{
	QUEUE_OK	= 0,
	QUEUE_FAIL	= 1
} QueueStatus;

/*---------------------------------------------------------------------------------------------
	Functions
---------------------------------------------------------------------------------------------*/
void	InitRingBuffer( RingBuffer_t *pQ );
bool	IsFull( RingBuffer_t *pQ );
bool	IsEmpty( RingBuffer_t *pQ );
U8		Enqueue( RingBuffer_t *pQ, int data );
U8		Dequeue(	RingBuffer_t *pQ, U8* pData );
U32		GetQueueSize( RingBuffer_t *pQ );
void	ClearQueue( RingBuffer_t *pQ );

/*---------------------------------------------------------------------------------------------
	Variables
---------------------------------------------------------------------------------------------*/
extern RingBuffer_t		rbUartRx;

#endif // __DATABUFF_H__