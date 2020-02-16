#include <stdio.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#define GB_JSON_PARSER_IMPL
#include "gb_json_parser.h"

using std::cout;
using std::endl;

std::string read_file_into_string(const std::string &filename)
{
    std::ifstream ifs(filename, std::ifstream::binary);

    std::stringstream buffer;
    buffer << ifs.rdbuf();

    return buffer.str();
}



int main(void)
{
    std::string input =
        R"FOO(
{
    "autosave" : true,
    "width" : 120,
    "height" : 30,
    "name" : "John",
    "numbers" : [1,2,3,4,5,6]
}
        )FOO";
 
    input = R"FOO(1)FOO";
    input = R"FOO([1, "2", 3.56, null, true, false, "\"in double quotes\""])FOO";

    input = read_file_into_string("data\\easy_test.json");
    
    input = read_file_into_string("data\\easy_test_2.json");

    input = read_file_into_string("data\\test.json");

    JSON_Value json = parse_json(input);

    return 0;
}
