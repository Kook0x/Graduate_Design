#ifndef _TYPES_H_
#define _TYPES_H_

#ifdef _MSC_VER  
/*  Signed.  */
typedef signed char		int8_t;
typedef short int		int16_t;
typedef int				int32_t;
typedef long int		int64_t;

/* Unsigned.  */
typedef unsigned char		uint8_t;//uint8_t
typedef unsigned short int	uint16_t;
typedef unsigned int		uint32_t;
typedef unsigned int		size_t;
typedef unsigned long int	uint64_t;

#else 
#include <stdint.h> 
#endif 


#endif