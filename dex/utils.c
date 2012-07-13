#include <stdio.h>
#include <stdint.h>

#include "utils.h"

#define MOD_ADLER 65521
uint32_t 
Adler32Checksum(unsigned char *data, size_t len)
{
	uint32_t a = 1, b = 0;
	size_t i;
	for(i = 0; i < len; i++)
	{
		a += data[i];
		while(a >= MOD_ADLER) a -= MOD_ADLER;
		b += a;
		while(b >= MOD_ADLER) b -= MOD_ADLER;
	}
	return (b<<16)|a;
}

uint32_t 
ReadULEB128(unsigned char **data)
{
	uint32_t result = 0;
	unsigned char *ptr = *data;
	int i;
	for(i = 0; i < 4; i++)
	{
		int cur = *(ptr++);
		result |= (cur & 0x7f) << (i*7);
		if(cur < 0x80)
			break;
	}
	*data = ptr;
	return result;
}
