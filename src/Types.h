/*****************************************************************//**
 * \file   Types.h
 * \brief  Shorthand type names for better syntax
 *
 * \author Iridescence - Nathan Bourgeois <iridescentrosesfall@gmail.com>
 * \date   November 2020
 *********************************************************************/
#pragma once
#include <stdint.h>

 /**
  * Shorthand types.
  */

#if CURRENT_PLATFORM != PLATFORM_PSP
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned long int u32;
typedef unsigned long long int u64;


typedef signed char s8;
typedef signed short s16;
typedef signed long int s32;
typedef signed long long int s64;
#endif

struct UUID {
	u64 id[2];

	bool operator==(const UUID& other) const {
		return (id[0] == other.id[0]) && (id[1] == other.id[1]);
	}
};