#include <stdio.h>

#include "gb_json.h"

int main()
{
    const char *input = "+3.34e+02";
    printf("str: '%s'  ", input);

    JSON_Value value = parse_json_number(&input);

    printf("num: '%f'  e: '%.2e'\n", value.number, value.number);

#if 0
    char *json_input = read_entire_file("data\\easy_test.json");
    Assert(json_input);

    printf("JSON to parse: %s\n", json_input);

    JSON_Value *json = parse_json(json_input);
#endif // 0


#if 0
    JSON_Object_Pair p1 = {};
    memcpy(&p1.key, "autosave", strlen("autosave"));
    p1.value.type = JSON_Type_True;

    JSON_Object_Pair p2 = {};
    memcpy(&p2.key, "width", strlen("width"));
    p2.value.type = JSON_Type_Number;
    p2.value.number = 120;

    JSON_Object_Pair p3 = {};
    memcpy(&p3.key, "name", strlen("name"));
    p3.value.type = JSON_Type_String;
    memcpy(&p3.value.string, "John", strlen("John"));


    JSON_Value *json = new_json_value(JSON_Type_Object);
    json->object->add_pair(&p1);
    json->object->add_pair(&p2);
    json->object->add_pair(&p3);

    free(json);
#endif // 0

    return 0;
}