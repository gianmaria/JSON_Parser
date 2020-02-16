#pragma once

#include <stdio.h>
#include <stdlib.h>

typedef signed char        s8;
typedef short              s16;
typedef int                s32;
typedef long long          s64;

typedef unsigned char      byte;
typedef unsigned char      u8;
typedef unsigned short     u16;
typedef unsigned int       u32;
typedef unsigned long long u64;

static_assert(sizeof(u64) == 8);


#define ArrayCount(a) (sizeof(a)/sizeof(a[0]))

#define Assert(cond, msg) if (!(cond)) {printf("[Assertion failed] \"%s\" at '%s':%d\n", msg, __FILE__, __LINE__); abort();}
