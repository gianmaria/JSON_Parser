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

enum class JSON_Token_Type : u32
{
    unknown,
    string, number, True, False, Null,
    object_begin, object_end,
    array_begin, array_end,
    colon, comma
};

struct JSON_Token
{
    JSON_Token_Type type = JSON_Token_Type::unknown;
    std::string text;
    double num = 0;
    u32 line = 0;
    u32 col = 0;
};

struct JSON_Tokenizer
{
    std::string input;
    std::queue<JSON_Token> tokens;
};

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
    if (*at && *at[0] == '-')
    {
        num_sign = -1;
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

    double json_num = 0;
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


    return json_num;
}

static JSON_Tokenizer tokenize_json(const std::string &input)
{
    JSON_Tokenizer tokenizer;
    tokenizer.input = input;

    const char *at = input.data();

    u32 line = 1;
    u32 col = 1;

    while (*at)
    {
        JSON_Token token;
        token.line = line;
        token.col = col;
#if 0

        if (*at == '\n')
        {
            col = 1;
            ++line;
            ++at;
            continue;
        }
        else if (*at == ' ' ||
                 *at == '\r' ||
                 *at == '\t')
        {
            while (*at == ' ' ||
                   *at == '\r' ||
                   *at == '\t')
            {
                ++col;
                ++at;
            }
            continue;
        }
        else if (*at == '{')
        {
            token.type = JSON_Token_Type::object_begin;
            token.text = "{";
        }
        else if (*at == '}')
        {
            token.type = JSON_Token_Type::object_end;
            token.text = "}";
        }
        else if (*at == '[')
        {
            token.type = JSON_Token_Type::array_begin;
            token.text = "[";
        }
        else if (*at == ']')
        {
            token.type = JSON_Token_Type::array_end;
            token.text = "]";
        }
        else if (*at == '"')
        {
            token.type = JSON_Token_Type::string;
            const char *begin = at;

            ++at; // skip "
            while (*at != '"')
            {
                if (at[0] == '\\' && at[1] == '"' || /* is: \" */
                    at[0] == '\\' && at[1] == '/' || /* is: \/ */
                    at[0] == '\\' && at[1] == '\\')  /* is: \\ */
                {
                    token.text += at[1];
                    at += 2;
                    continue;
                }

                token.text += *at;
                ++at;
            }

            ++at; // skip last "
            col += (at - begin); // JSON string can only be on one line 
            tokenizer.tokens.push(std::move(token));
            continue;
        }
        else if (*at == ':')
        {
            token.type = JSON_Token_Type::colon;
            token.text = ":";
        }
        else if (*at == ',')
        {
            token.type = JSON_Token_Type::comma;
            token.text = ",";
        }
        else if (*at == 't')
        {
            std::string target = "true";
            std::string at_s(at, target.length());

            if (target == at_s)
            {
                token.type = JSON_Token_Type::True;
                token.text = target;

                at += target.length();
                col += target.length();
                tokenizer.tokens.push(std::move(token));
                continue;

            }
            // handle error
            Assert(false, "Expected true!");
        }
        else if (*at == 'f')
        {
            std::string target = "false";
            std::string at_s(at, target.length());

            if (target == at_s)
            {
                token.type = JSON_Token_Type::False;
                token.text = target;

                at += target.length();
                col += target.length();
                tokenizer.tokens.push(std::move(token));
                continue;

            }
            // handle error
            Assert(false, "Expected false!");
        }
        else if (*at == 'n')
        {
            std::string target = "null";
            std::string at_s(at, target.length());

            if (target == at_s)
            {
                token.type = JSON_Token_Type::Null;
                token.text = target;

                at += target.length();
                col += target.length();
                tokenizer.tokens.push(std::move(token));
                continue;
            }
            // handle error
            Assert(false, "Expected null!");
        }
        else if (is_number(at))
        {
            const char *begin = at;
            double num = parse_json_number(&at);

            token.type = JSON_Token_Type::number;
            token.text = std::string(begin, (u64)(at - begin));
            token.num = num;

            col += (at - begin); // JSON number can only be on one line 
            tokenizer.tokens.push(std::move(token));
            continue;
        }
        else
        {
            Assert(false, "Unrecognized charater!");
            break;
        }

#endif // 0

        ++at;
        ++col;

        tokenizer.tokens.push(std::move(token));
    }

    return tokenizer;
}


static JSON_Value parse_json_value(std::queue<JSON_Token> &tokens);

static JSON_Value parse_json_array(std::queue<JSON_Token>& tokens)
{
    JSON_Value json;
    json.type = JSON_Type_Array;
    json.array = new JSON_Array();
    //json.array->data.reserve(50);

    while (tokens.front().type != JSON_Token_Type::array_end)
    {
        JSON_Value value = parse_json_value(tokens);
        json.array->data.push_back(std::move(value));

        if (tokens.front().type == JSON_Token_Type::comma)
        {
            tokens.pop();
        }
    }

    return json;
}

static JSON_Value parse_json_object(std::queue<JSON_Token> &tokens)
{
    JSON_Value json;
    json.type = JSON_Type_Object;
    json.object = new JSON_Object();
    //json.object->data.reserve(50);

    while (tokens.front().type != JSON_Token_Type::object_end)
    {
        JSON_Object_Pair pair;

        Assert(tokens.front().type == JSON_Token_Type::string, "Expected string as key in json object");
        pair.key = tokens.front().text;
        tokens.pop();

        Assert(tokens.front().type == JSON_Token_Type::colon, "Expected colon in json object");
        tokens.pop();

        pair.value = parse_json_value(tokens);

        json.object->data.push_back(std::move(pair));

        if (tokens.front().type == JSON_Token_Type::comma)
        {
            tokens.pop();
        }
    }

    return json;
}

static JSON_Value parse_json_value(std::queue<JSON_Token> &tokens)
{
    JSON_Value json;

    const JSON_Token &token = tokens.front();

    switch (token.type)
    {
        case JSON_Token_Type::string:
        {
            json.type = JSON_Type_String;
            json.string = token.text;
        } break;

        case JSON_Token_Type::True:
        {
            json.type = JSON_Type_True;
        } break;

        case JSON_Token_Type::False:
        {
            json.type = JSON_Type_False;
        } break;

        case JSON_Token_Type::Null:
        {
            json.type = JSON_Type_Null;
        } break;

        case JSON_Token_Type::number:
        {
            json.type = JSON_Type_Number;
            json.number = token.num;
        } break;

        case JSON_Token_Type::array_begin:
        {
            tokens.pop();
            json = parse_json_array(tokens);
        } break;

        case JSON_Token_Type::object_begin:
        {
            tokens.pop();
            json = parse_json_object(tokens);
        } break;

        case JSON_Token_Type::object_end:
        case JSON_Token_Type::array_end:
        case JSON_Token_Type::colon:
        case JSON_Token_Type::comma:
        case JSON_Token_Type::unknown:
        {
            Assert(false, "Current token was unexpected!");
        } break;

        default:
        {
            Assert(false, "WTF?!?!");
        } break;
    }

    tokens.pop();

    return json;
}



static JSON_Value parse_json(const std::string &source)
{
    JSON_Tokenizer tokenizer = tokenize_json(source);

    JSON_Value json = parse_json_value(tokenizer.tokens);

    return json;
}


void print_json(JSON_Value json)
{

}

#endif