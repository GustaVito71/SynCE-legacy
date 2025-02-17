/* $Id: bswap.c 3983 2011-03-21 17:23:56Z mark_ellis $ */

#include "synce.h"

/*
 * Written by Manuel Bouyer <bouyer@netbsd.org>.
 * Public domain.
 */

uint16_t bswap_16(uint16_t x)
{
	return ((x << 8) & 0xff00) | ((x >> 8) & 0x00ff);
}
	
uint32_t bswap_32(uint32_t x)
{
	return	((x << 24) & 0xff000000 ) |
		((x <<  8) & 0x00ff0000 ) |
		((x >>  8) & 0x0000ff00 ) |
		((x >> 24) & 0x000000ff );
}

