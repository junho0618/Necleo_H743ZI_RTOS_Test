#ifndef __TYPEDEF_H
#define __TYPEDEF_H

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------
	includes
----------------------------------------------------------------------*/
#include <stdbool.h>	

/*----------------------------------------------------------------------
	typedefs
----------------------------------------------------------------------*/
typedef	bool					BOOL;
typedef	unsigned char			U8;
typedef	unsigned short			U16;
typedef	unsigned int			U32;
typedef	volatile unsigned char	VU8;
typedef	volatile unsigned short	VU16;
typedef	volatile unsigned int	VU32;
typedef	signed	char			S8;
typedef	signed short			S16;
typedef	signed int				S32;

typedef enum
{
	INIT_OK		= 0,
	INIT_FAIL	= 1
} InitStatus;

/*----------------------------------------------------------------------
	defines
----------------------------------------------------------------------*/
#ifndef FALSE
	#define FALSE				false
	#define TRUE				true
#endif

#ifdef __cplusplus
}
#endif

#endif /* __TYPEDEF_H */