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
        char string[6];
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
    JSON_Object_Pair data[8];
    u32 count;
};

void add_pair(JSON_Object *obj, const JSON_Object_Pair *pair)
{
    if (obj->count < ArrayCount(obj->data))
    {
        obj->data[obj->count++] = *pair;
    }
    else
    {
        Assert(!"JSON_Object is full!");
    }
}

struct JSON_Array
{
    JSON_Value data[8];
    u32 count;
};

void add(JSON_Array *arr, const JSON_Value *value)
{
    if (arr->count < ArrayCount(arr->data))
    {
        arr->data[arr->count++] = *value;
    }
    else
    {
        Assert(!"JSON_Array is full!");
    }
}


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



JSON_Value parse_json(const char *source);

JSON_Value parse_json_value(const char **source);

JSON_Value parse_json_array(const char **source);

JSON_Value parse_json_string(const char **at)
{
    JSON_Value json_value = { };
    json_value.type = JSON_Type_String;

    u32 str_idx = 0;

    if (*at)
    {
        Assert(*at[0] == '"');
        ++*at;

        while (*at[0] != '"')
        {
            if (!(strlen(json_value.string) < sizeof(json_value.string) - 1))
            {
                break;
            }
            json_value.string[str_idx++] = *at[0];
            ++*at;
        }

        ++*at; // skip closing "
    }

    return json_value;
}

JSON_Value parse_json_true(const char **at)
{
    JSON_Value json_value = { };

    const char *target = "true";

    if (strlen(*at) >= strlen(target))
    {
        if (memcmp(*at, target, strlen(target)) == 0)
        {
            json_value.type = JSON_Type_True;
        }
    }

    *at += strlen(target);
    return json_value;
}

JSON_Value parse_json_false(const char **at)
{
    JSON_Value json_value = { };

    const char *target = "false";

    if (strlen(*at) >= strlen(target))
    {
        if (memcmp(*at, target, strlen(target)) == 0)
        {
            json_value.type = JSON_Type_False;
        }
    }

    *at += strlen(target);
    return json_value;
}

JSON_Value parse_json_null(const char **at)
{
    JSON_Value json_value = { };

    const char *target = "null";

    if (strlen(*at) >= strlen(target))
    {
        if (memcmp(*at, target, strlen(target)) == 0)
        {
            json_value.type = JSON_Type_Null;
        }
    }
    *at += strlen(target);
    return json_value;
}

bool is_digit(char c)
{
    return (c >= '0' && c <= '9') ? true : false;
}

JSON_Value parse_json_number(const char **at)
{
    JSON_Value json_value = { };

    double json_num = 0;

    s8 num_sign = 1;
    if (*at && *at[0] == '-')
    {
        num_sign = -1;
        ++*at;
    }
    else if (*at && *at[0] == '+')
    {
        ++*at;
    }

    u64 number = 0;
    while (*at && is_digit(*at[0]))
    {
        number *= 10;
        number += (*at[0] - '0');
        ++*at;
    }

    u64 fraction = 0;
    u64 power = 1;
    if (*at && *at[0] == '.')
    {
        ++*at;

        while (*at && is_digit(*at[0]))
        {
            fraction *= 10;
            fraction += (*at[0] - '0');
            power *= 10;
            ++*at;
        }
    }

    u64 exp = 0;
    s8 exp_sign = 1;
    if (*at && (*at[0] == 'e' || *at[0] == 'E'))
    {
        ++*at;

        if (*at && *at[0] == '-')
        {
            exp_sign = -1;
            ++*at;
        }
        else if (*at && *at[0] == '+')
        {
            ++*at;
        }

        while (*at && is_digit(*at[0]))
        {
            exp *= 10;
            exp += (*at[0] - '0');
            ++*at;
        }
    }

    json_num = number;
    json_num += (double)fraction / (double)power;

    if (exp_sign > 0)
    {
        json_num *= pow(10, exp);
    }
    else
    {
        json_num /= pow(10, exp);
    }

    json_num *= num_sign;


    json_value.type = JSON_Type_Number;
    json_value.number = json_num;

    return json_value;
}

JSON_Value parse_json_array(const char **at)
{
    JSON_Value json = {};
    json.type = JSON_Type_Array;
    json.array = (JSON_Array*)calloc(1, sizeof(JSON_Array));

    Assert(*at[0] == '[');
    ++*at;

    bool done = false;

    while (*at && !done)
    {
        switch (*at[0])
        {
            case ' ':
            {
                ++*at;
            } break;

            case ']':
            {
                ++*at;
                done = true;
            } break;

            case ',':
            {
                ++*at;
            } break;

            default:
            {
                JSON_Value val = parse_json_value(at);
                add(json.array, &val);
            } break;
        }
    }

    return json;
}




JSON_Value parse_json_value(const char **at)
{
    JSON_Value res;

    switch (*at[0])
    {
        case ' ':
        {
            ++*at;
        } break;

        case '{': // object
        {
            // return parse_json_object(&c);
        } break;

        case '[': // array
        {
            res = parse_json_array(at);
        } break;

        case '"': // string
        {
            res = parse_json_string(at);
        } break;

        case 't': // true
        {
            res = parse_json_true(at);
        } break;

        case 'f': // false
        {
            res = parse_json_false(at);
        } break;

        case 'n': // null
        {
            res = parse_json_null(at);
        } break;

        default: // number
        {
            res = parse_json_number(at);
        } break;
    }

    return res;
}

JSON_Value parse_json(const char *source)
{
    JSON_Value json = parse_json_value(&source);

    return json;
}
