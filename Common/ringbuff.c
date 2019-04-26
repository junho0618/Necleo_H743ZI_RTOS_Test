/*************************************************************
 * NOTE	: ringbuff.c
 *		Ring Buffer
 * Author :	Lee	junho
 * Since : 2019.04.12
**************************************************************/

#include "ringbuff.h"
#include "debug.h"

RingBuffer_t	rbUartRx;
	 
void InitRingBuffer( RingBuffer_t *pQ )
{
	ClearQueue( pQ );
}

bool IsFull( RingBuffer_t *pQ )
{
	if(	pQ->count ==	QUEUE_SIZE )
		return true;
	else
		return false;
}

bool IsEmpty( RingBuffer_t *pQ )
{
	return ( pQ->count == 0 ? true : false );
}

U8 Enqueue( RingBuffer_t *pQ, int data )
{
	if(	IsFull(pQ) )
	{
		jeprintf("Queue	is Full\r\n");
		return QUEUE_FAIL;
	}

	pQ->buf[pQ->rear]	= data;
	pQ->rear			= NEXT(pQ->rear);
	pQ->count++;
	
	return QUEUE_OK;
}

U8 Dequeue(	RingBuffer_t *pQ, U8* pData )
{
	if(	IsEmpty(pQ) )
	{
		jeprintf("Queue	is Empty\r\n");
		return QUEUE_FAIL;
	}
	
	*pData		= pQ->buf[pQ->front];
	pQ->front	= NEXT(pQ->front);
	pQ->count--;
	
	return QUEUE_OK;
}

U32	GetQueueSize( RingBuffer_t *pQ )
{	
	return pQ->count;
}

void ClearQueue( RingBuffer_t *pQ )
{
	pQ->front	= 0;
	pQ->rear	= 0;
	pQ->count	= 0;
}