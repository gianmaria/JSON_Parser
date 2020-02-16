#ifndef GB_JSON_PARSER_H
#define GB_JSON_PARSER_H

#include <string>
#include <vector>

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

JSON_Value parse_json(const std::string &source);
JSON_Value parse_json_file(const std::string &filename);


#endif

#ifdef GB_JSON_PARSER_IMPL

enum class JSON_Token_Type : u32
{
    unknown,
    string, number, True, False, Null,
    object_begin, object_end,
    array_begin, array_end,
    colon, comma,
    end_of_tokens
};

struct JSON_Token
{
    JSON_Token_Type type = JSON_Token_Type::unknown;
    std::string text = "unknown";
    double num = 0;
    u32 line = 0;
    u32 col = 0;
};

struct JSON_Tokenizer
{
    std::string input;
    std::vector<JSON_Token> tokens;
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

static u32 eat_whitespaces(const char **at)
{
    u32 skipped = 0;

    while (*at[0] == ' ' ||
           *at[0] == '\r' ||
           *at[0] == '\t')
    {
        ++skipped;
        ++*at;
    }

    return skipped;
}

static JSON_Tokenizer tokenize_json(const std::string &input)
{
    JSON_Tokenizer tokenizer;
    tokenizer.tokens.reserve(1000); // @TODO: tune this parameter?
    tokenizer.input = input;

    const char *at = input.data();

    u32 line = 1;
    u32 col = 1;

    while (at && *at)
    {
        col += eat_whitespaces(&at);

        JSON_Token token;
        token.line = line;
        token.col = col;

        if (*at == '\n')
        {
            col = 1;
            ++line;
            ++at;
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

            ++at;
            while (*at != '"')
            {
                if (at[0] == '\\' && at[1] == '"' || /* is: \" */
                    at[0] == '\\' && at[1] == '/' || /* is: \/ */
                    at[0] == '\\' && at[1] == '\\') /* is: \\ */
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
            tokenizer.tokens.push_back(std::move(token));
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
                tokenizer.tokens.push_back(std::move(token));
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
                tokenizer.tokens.push_back(std::move(token));
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
                tokenizer.tokens.push_back(std::move(token));
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
            tokenizer.tokens.push_back(std::move(token));
            continue;
        }
        else
        {
            Assert(false, "Unrecognized charater!");
            break;
        }

        ++at;
        ++col;

        tokenizer.tokens.push_back(std::move(token));
    }

    JSON_Token end_of_tokens;
    end_of_tokens.type = JSON_Token_Type::end_of_tokens;
    end_of_tokens.text = "end_of_tokens";

    tokenizer.tokens.emplace_back(end_of_tokens);

    return tokenizer;
}

#endif