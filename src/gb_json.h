#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define ArrayCount(a) (sizeof(a)/sizeof(a[0]))
#define Assert(cond) if (!(cond)) (*(int*)0 = 0)

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

enum JSON_Type
{
    JSON_Type_Unknown,
    JSON_Type_Object,
    JSON_Type_Array,
    JSON_Type_String,
    JSON_Type_Number,
    JSON_Type_True,
    JSON_Type_False,
    JSON_Type_Null
};

struct JSON_Object;
struct JSON_Array;

struct JSON_Value
{
    JSON_Type type;
    union
    {
        JSON_Object *object;
        JSON_Array *array;
        char string[64];
        double number;
    };
};

struct JSON_Object_Pair
{
    char key[64];
    JSON_Value value;
};

struct JSON_Object
{
    JSON_Object_Pair data[128];
    u32 count;

    void add_pair(const JSON_Object_Pair *pair)
    {
        if (count < ArrayCount(data))
        {
            data[count++] = *pair;
        }
        else
        {
            Assert(!"JSON_Object is full!");
        }
    }
};

struct JSON_Array
{
    JSON_Value data[128];
    u32 count;

    void add(const JSON_Value *value)
    {
        if (count < ArrayCount(data))
        {
            data[count++] = *value;
        }
        else
        {
            Assert(!"JSON_Array is full!");
        }
    }
};


char *read_entire_file(const char *file_path)
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

JSON_Value *new_json_value(enum JSON_Type type)
{
    JSON_Value *value = 0;

    value = (JSON_Value *)malloc(sizeof(JSON_Value));

    if (value)
    {
        memset(value, 0, sizeof(JSON_Value));
        value->type = type;

        switch (value->type)
        {
            case JSON_Type_Object:
            {
                value->object = (JSON_Object *)malloc(sizeof(JSON_Object));
                memset(value->object, 0, sizeof(JSON_Object));
            } break;
        }
    }

    return value;
}


JSON_Value parse_json_string(char **source)
{
    JSON_Value json_value = { };
    json_value.type = JSON_Type_String;

    char *c = *source;
    u32 str_idx = 0;

    if (c)
    {
        Assert(*c == '"');
        ++c;

        while (*c != '"')
        {
            json_value.string[str_idx++] = *c;
        }

        ++c; // skip closing "

        *source = c;
    }

    return json_value;
}

JSON_Value parse_json_true(char **source)
{
    JSON_Value json_value = { };

    char *c = *source;

    if (strlen(c) >= strlen("true"))
    {
        if (memcmp(c, "true", strlen("true")) == 0)
        {
            json_value.type = JSON_Type_True;
        }
    }

    return json_value;
}

JSON_Value parse_json_false(char **source)
{
    JSON_Value json_value = { };

    char *c = *source;

    if (strlen(c) >= strlen("false"))
    {
        if (memcmp(c, "false", strlen("false")) == 0)
        {
            json_value.type = JSON_Type_False;
        }
    }

    return json_value;
}

JSON_Value parse_json_null(char **source)
{
    JSON_Value json_value = { };

    char *c = *source;

    if (strlen(c) >= strlen("null"))
    {
        if (memcmp(c, "null", strlen("null")) == 0)
        {
            json_value.type = JSON_Type_Null;
        }
    }

    return json_value;
}

bool is_digit(char c)
{
    return (c >= '0' && c <= '9') ? true : false;
}

JSON_Value parse_json_number(const char **source)
{
    JSON_Value json_value = { };

    double json_num = 0;

    const char *c = *source;

    s32 num_sign = 1;
    if (c && *c == '-')
    {
        num_sign = -1;
        ++c;
    }
    else if (c && *c == '+')
    {
        ++c;
    }

    u32 number = 0;
    while (c && is_digit(*c))
    {
        number *= 10;
        number += (*c - '0');
        ++c;
    }

    u32 fraction = 0;
    u32 power = 1;
    if (c && *c == '.')
    {
        ++c;

        while (c && is_digit(*c))
        {
            fraction *= 10;
            fraction += (*c - '0');
            power *= 10;
            ++c;
        }
    }

    u32 exp = 0;
    s32 exp_sign = 1;
    if (c && (*c == 'e' || *c == 'E'))
    {
        ++c;

        if (c && *c == '-')
        {
            exp_sign = -1;
            ++c;
        }
        else if (c && *c == '+')
        {
            ++c;
        }

        while (c && is_digit(*c))
        {
            exp *= 10;
            exp += (*c - '0');
            ++c;
        }
    }

    json_num = (double)number;
    json_num += (double)fraction / (double)power;

    if (exp_sign > 0)
    {
        json_num *= pow(10, exp);
    }
    else
    {
        json_num /= pow(10, exp);
    }

    json_num *= (double)num_sign;


    *source = c;

    json_value.type = JSON_Type_Number;
    json_value.number = json_num;

    return json_value;
}

JSON_Value* parse_json(const char *source)
{
    JSON_Value *json = (JSON_Value*)malloc(sizeof(JSON_Value));

    const char *c = source;

    while (c)
    {
        while (*c == ' ')
        {
            ++c;
        }

        switch (*c)
        {
            case '{': // object
            {
                json->type = JSON_Type_Object;
                json->object = (JSON_Object*)malloc(sizeof(JSON_Object));
                //parse_json_object(c, json);
            } break;

            case '[': // array
            {
                //parse_array()
            } break;

            case '"': // string
            {

            } break;

            case 't': // true
            {

            } break;

            case 'f': // false
            {

            } break;

            case 'n': // null
            {

            } break;

            default: // number
            {

            }
        }

        ++c;
    }

    return json;
}
