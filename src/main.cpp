#include <stdio.h>

#include <fstream>
#include <sstream>
#include <string>

#include "gb_utils.h"
#include "gb_json.h"
#include "gb_json_parser.h"


int main(void)
{
    std::ifstream t("data\\easy_test.json", std::ifstream::binary);
    
    std::stringstream buffer;
    buffer << t.rdbuf();
    
    std::string file_content = buffer.str();

    const char *input =
        R"FOO(
{
    "autosave" : true,
    "width" : 120,
    "height" : 30,
    "name" : "John",
    "numbers" : [1,2,3,4,5,6]
}
        )FOO";
 
    JSON_Tokenizer tokenizer = tokenize_json(file_content);



    return 0;
}

int main2(void)
{

    char *input = read_entire_file_in_memory("data/test.json");

    JSON_Value value = parse_json(input);

#if 0
    const char *input = "+9.94758600e+2";
    printf("input str: '%s'  ", input);

    JSON_Value value = parse_json_number(&input);

    printf("num: '%f'\n        e: '%+.8e'\n", value.number, value.number);

#endif // 0


#if 0
    const char *input = 
        R"FOO(
{
    "autosave" : true,
    "width" : 120,
    "height" : 30,
    "name" : "John",
    "numbers" : [1,2,3,4,5,6]
}
        )FOO";

    JSON_Value value = parse_json(input);

#endif // 0

    int stop = 0;

#if 0
    char *json_input = read_entire_file("data\\easy_test.json");
    Assert(json_input);

    printf("JSON to parse: %s\n", json_input);

    JSON_Value *json = parse_json(json_input);
#endif // 0


    return 0;
}