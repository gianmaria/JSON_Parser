#pragma once

#include <string>
#include <vector>

#include "gb_utils.h"

enum class JSON_Token_Type
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
    std::string content = "";
    u32 line = 0;
    u32 col = 0;
};

struct JSON_Tokenizer
{
    std::string input;
    std::vector<JSON_Token> tokens;
};

bool is_number(const char *c)
{

}

JSON_Tokenizer tokenize_json(const std::string &input)
{
    JSON_Tokenizer tokenizer;
    //tokenizer.tokens.reserve(100); // @TODO: tune this parameter?
    tokenizer.input = input;

    const char *at = input.data();

    u32 line = 1;
    u32 col = 1;

    while (at && *at)
    {
        while (*at == ' ' ||
               *at == '\r' ||
               *at == '\t')
        {
            ++col;
            ++at;
        }

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
            token.content = "{";
        }
        else if (*at == '}')
        {
            token.type = JSON_Token_Type::object_end;
            token.content = "}";
        }
        else if (*at == '[')
        {
            token.type = JSON_Token_Type::array_begin;
            token.content = "[";
        }
        else if (*at == ']')
        {
            token.type = JSON_Token_Type::array_end;
            token.content = "]";
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
                    token.content += at[1];
                    at += 2;
                    continue;
                }

                token.content += *at;
                ++at;
            }

            ++at; // skip last "
            col += (u32)(at - begin);
            tokenizer.tokens.push_back(std::move(token));
            continue;
        }
        else if (*at == ':')
        {
            token.type = JSON_Token_Type::colon;
            token.content = ":";
        }
        else if (*at == ',')
        {
            token.type = JSON_Token_Type::comma;
            token.content = ",";
        }
        else if (*at == 't')
        {
            token.type = JSON_Token_Type::True;
            token.content = "true";

            if (memcmp(at, "true", 4) != 0)
            {
                // handle error
                Assert(false, "Expected true!");
            }

            at += 4;
            col += 4;
            tokenizer.tokens.push_back(std::move(token));
            continue;
        }
        else if (is_numebr(at))
        {

        }
        else
        {
            Assert(false, "Character not recognized!");
        }

        ++at;
        ++col;

        tokenizer.tokens.push_back(std::move(token));
    }

    return tokenizer;
}