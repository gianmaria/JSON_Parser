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


char *read_entire_file_in_memory(const char *file_path)
{
    char *res = 0;

    FILE *file = fopen(file_path, "rb");

    if (file)
    {
        fseek(file, 0, SEEK_END);
        u64 file_size = (u64)ftell(file);
        rewind(file);

        res = (char *)malloc(file_size + 1);

        if (res)
        {
            u64 read = fread(res, 1, file_size, file);

            if (read == file_size)
            {
                res[file_size] = 0;
            }
            else
            {
                free(res);
                res = 0;
            }

            fclose(file);
        }
    }

    return res;
}
