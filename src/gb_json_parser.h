#ifndef GB_JSON_PARSER_H
#define GB_JSON_PARSER_H

#include <math.h>

#include <string>
#include <vector>
#include <queue>

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

#ifdef NDEBUG
#define Assert(cond, msg) (void)0;
#else
#define Assert(cond, msg) if (!(cond)) {printf("[Assertion failed] \"%s\" at '%s':%d\n", msg, __FILE__, __LINE__); *(u32*)0xdeadbeefdeadbeef=0;}
#endif

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
    JSON_Type type = JSON_Type_Unknown;

    JSON_Object* object = nullptr;
    JSON_Array* array = nullptr;
    std::string string;
    double number = 0;
};

struct JSON_Object_Pair
{
    std::string key;
    JSON_Value value;
};

struct JSON_Object
{
    std::vector<JSON_Object_Pair> data;
};

struct JSON_Array
{
    std::vector<JSON_Value> data;
};

static JSON_Value parse_json(const std::string &source);
//JSON_Value parse_json_file(const std::string &filename);


#endif

#ifdef GB_JSON_PARSER_IMPL

static void eat_json_whitespaces(const char **at)
{
    while (*at)
    {
        if ((*at)[0] == '\r' &&
            (*at)[1] == '\n')
        {
            //col = 1;
            //++line;
            *at += 2;
        }
        else if ((*at)[0] == '\n')
        {
            //col = 1;
            //++line;
            ++*at;
        }
        else if ((*at)[0] == '\t')
        {
            //++col;
            ++*at;
        }
        else if ((*at)[0] == ' ')
        {
            //++col;
            ++*at;
        }
        else
        {
            break;
        }
    }
}

static bool is_digit(char c)
{
    return (c >= '0' && c <= '9') ? true : false;
}

static bool is_number(const char *c)
{
    bool is_number = false;

    if (is_digit(*c) ||
        c[0] == '-' && is_digit(c[1]))
    {
        is_number = true;
    }

    return is_number;
}

static double parse_json_number(const char **at)
{
    s8 num_sign = 1;
    if (*at && 
        (*at)[0] == '-')
    {
        num_sign = -1;
        ++*at;
    }

    u64 number = 0;
    while (*at && 
           is_digit((*at)[0]))
    {
        number *= 10;
        number += ((*at)[0] - '0');
        ++*at;
    }

    u64 fraction = 0;
    u64 power = 1;
    if (*at && 
        (*at)[0] == '.')
    {
        ++*at;

        while (*at && 
               is_digit((*at)[0]))
        {
            fraction *= 10;
            fraction += ((*at)[0] - '0');
            power *= 10;
            ++*at;
        }
    }

    u64 exp = 0;
    s8 exp_sign = 1;
    if (*at && 
        ((*at)[0] == 'e' || (*at)[0] == 'E') )
    {
        ++*at;

        if (*at && 
            (*at)[0] == '-')
        {
            exp_sign = -1;
            ++*at;
        }
        else if (*at && 
            (*at)[0] == '+')
        {
            ++*at;
        }

        while (*at && 
               is_digit((*at)[0]))
        {
            exp *= 10;
            exp += ((*at)[0] - '0');
            ++*at;
        }
    }

    double json_num = number;
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

    return json_num;
}

// @NOTE: supported ascii input only
static void parse_json_string(const char **at, JSON_Value *json)
{
    json->type = JSON_Type_String;

    ++*at; // skip opening "

    while (*at && 
           (*at)[0] != '"')
    {
        const char c = (*at)[0];

        switch (c)
        {
            case '\\':
            {
                const char c_next = (*at)[1];

                switch (c_next)
                {
                    case '"':
                    {
                        json->string.append(&c_next, 1);
                        *at += 2;
                    } break;

                    case '\\':
                    {
                        json->string.append(&c_next, 1);
                        *at += 2;
                    } break;

                    case '/':
                    {
                        json->string.append(&c_next, 1);
                        *at += 2;
                    } break;

                    case 'b':
                    {
                        json->string.append(&c, 1);
                        json->string.append(&c_next, 1);
                        *at += 2;
                    } break;

                    case 'f':
                    {
                        json->string.append(&c, 1);
                        json->string.append(&c_next, 1);
                        *at += 2;
                    } break;

                    case 'n':
                    {
                        json->string.append(&c, 1);
                        json->string.append(&c_next, 1);
                        *at += 2;
                    } break;

                    case 'r':
                    {
                        json->string.append(&c, 1);
                        json->string.append(&c_next, 1);
                        *at += 2;
                    } break;

                    case 't':
                    {
                        json->string.append(&c, 1);
                        json->string.append(&c_next, 1);
                        *at += 2;
                    } break;

                    case 'u': // @NOTE: not yet supported
                    {
                        *at += 2; /* \u */
                        *at += 4; /* xxxx */
                    } break;

                    default:
                    {
                        Assert(false, "Invalid escape sequence");
                    }
                }

            } break;

            default:
            {
                json->string.append(&c, 1);
                ++*at;
            }
        }

    }

    ++*at; // skip closing "
}

static void parse_json_value(const char** at, JSON_Value* json);


static void parse_json_array(const char** at, JSON_Value* json)
{
    json->type = JSON_Type_Array;
    json->array = new JSON_Array();

    ++*at; // skip [

    eat_json_whitespaces(at);

    while (*at &&
           (*at)[0] != ']')
    {
        JSON_Value value;
        parse_json_value(at, &value);
        json->array->data.push_back(std::move(value));

        eat_json_whitespaces(at);

        if ((*at)[0] == ',')
        {
            ++*at;
        }

        eat_json_whitespaces(at);
    }

    ++*at; // skip ]
}

static void parse_json_object(const char** at, JSON_Value* json)
{
    json->type = JSON_Type_Object;
    json->object = new JSON_Object();

    ++*at; // skip {

    eat_json_whitespaces(at);

    while (*at &&
            (*at)[0] != '}')
    {
        JSON_Object_Pair pair;

        JSON_Value value;
        parse_json_string(at, &value);
        Assert(value.type == JSON_Type_String, "Expected string as key in object");

        pair.key = std::move(value.string);

        eat_json_whitespaces(at);

        Assert((*at)[0] == ':', "Expected colon as json object separator");
        ++*at;

        eat_json_whitespaces(at);

        parse_json_value(at, &pair.value);

        json->object->data.push_back(std::move(pair));

        eat_json_whitespaces(at);

        if ((*at)[0] == ',')
        {
            ++*at;
        }
        eat_json_whitespaces(at);
    }

    ++*at; // skip }
}




static void parse_json_value(const char** at, JSON_Value* json)
{
    eat_json_whitespaces(at);

    char c = (*at)[0];

    switch(c)
    {
        case '{':
        {
            parse_json_object(at, json);
        } break;

        case '[':
        {
            parse_json_array(at, json);
        } break;

        case '"':
        {
            parse_json_string(at, json);
        } break;

        case 't':
        case 'f':
        case 'n':
        {
            if (strlen(*at) >= 4 && 
                memcmp(*at, "true", 4) == 0)
            {
                json->type = JSON_Type_True;
                *at += 4;
            }
            else if (strlen(*at) >= 5 && 
                     memcmp(*at, "false", 5) == 0)
            {
                json->type = JSON_Type_False;
                *at += 5;
            }
            else if (strlen(*at) >= 4 && 
                     memcmp(*at, "null", 4) == 0)
            {
                json->type = JSON_Type_Null;
                *at += 4;
            }
            else
            {
                Assert(false, "Expected true, false or null");
            }

        } break;

        default:
        {
            if (is_number(*at)) // all numbers are treated as double
            {
                json->type = JSON_Type_Number;
                json->number = parse_json_number(at);
            }
            else
            {
                Assert(false, "Expected number!");
            }
        }
    }

}


static void parse_json(const char* source, JSON_Value *json)
{
    parse_json_value(&source, json);
}


void print_json(JSON_Value json)
{

}

#endif