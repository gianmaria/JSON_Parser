#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "gb_utils.h"

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
};

void add_pair(JSON_Object *obj, const JSON_Object_Pair *pair)
{
    if (obj->count < ArrayCount(obj->data))
    {
        obj->data[obj->count++] = *pair;
    }
    else
    {
        Assert(false, "JSON_Object is full!");
    }
}

struct JSON_Array
{
    JSON_Value data[128];
    u32 count;
};

void push_back(JSON_Array *arr, const JSON_Value *value)
{
    if (arr->count < ArrayCount(arr->data))
    {
        arr->data[arr->count++] = *value;
    }
    else
    {
        Assert(false, "JSON_Array is full!");
    }
}


JSON_Value parse_json(const char *source);

JSON_Value parse_json_value(const char **source);

JSON_Value parse_json_array(const char **source);

static void eat_whitespaces(const char **at)
{
    if (*at)
    {
        while (*at[0] == ' ' ||
               *at[0] == '\n' ||
               *at[0] == '\r' ||
               *at[0] == '\t')
        {
            ++*at;
        }
    }
}

JSON_Value parse_json_string(const char **at)
{
    JSON_Value json_value = { };
    json_value.type = JSON_Type_String;

    u32 str_idx = 0;

    if (*at)
    {
        Assert(*at[0] == '"', "");
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

    Assert(*at[0] == '[', "");
    ++*at;

    bool done = false;

    while (*at && !done)
    {
        eat_whitespaces(at);

        switch (*at[0])
        {
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
                push_back(json.array, &val);
            } break;
        }
    }

    return json;
}

JSON_Value parse_json_object(const char **at)
{
    JSON_Value json;
    json.type = JSON_Type_Object;
    json.object = (JSON_Object*)calloc(1, sizeof(JSON_Object));

    Assert(*at[0] == '{', "");
    ++*at;

    if (*at)
    {
        while (*at[0] != '}')
        {
            eat_whitespaces(at);

            JSON_Object_Pair pair = {};
    
            JSON_Value key = parse_json_string(at);
            memcpy(pair.key, key.string, strlen(key.string));

            eat_whitespaces(at);

            Assert(*at[0] == ':', "");
            ++*at;

            eat_whitespaces(at);

            pair.value = parse_json_value(at);

            add_pair(json.object, &pair);

            eat_whitespaces(at);

            if (*at[0] == ',')
            {
                ++*at;
            }
        }
    }


    return json;
}


JSON_Value parse_json_value(const char **at)
{
    JSON_Value res;

    eat_whitespaces(at);

    switch (*at[0])
    {
        case '{': // object
        {
            res = parse_json_object(at);
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

        default: // number, probably :)
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
