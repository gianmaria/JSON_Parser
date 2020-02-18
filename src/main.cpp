#include <stdio.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include <exception>
#include <chrono>
using namespace std::chrono;

#include "json.hpp"
using nlohmann::json;

using std::cout;
using std::endl;

#define GB_JSON_PARSER_IMPL
#include "gb_json_parser.h"


std::string read_file_into_string(const std::string &filename)
{
    std::ifstream ifs(filename, std::ifstream::binary);

    std::stringstream buffer;
    buffer << ifs.rdbuf();

    return buffer.str();
}

char *read_entire_file_in_memory(const char *file_path)
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


#include <type_traits>

struct Foo
{
    int* k;

    Foo() = delete;

    ~Foo() = default;
    
};

template<class T>
void test_things(const std::string &name)
{
    cout << std::boolalpha;

    cout << name << " is: " << endl;;
    
    cout << "    constructible: " << std::is_constructible<T>::value << endl;
    cout << "    trivially_constructible: " << std::is_trivially_constructible<T>::value << endl; 
    cout << "    nothrow_constructible: " << std::is_nothrow_constructible<T>::value << endl;

    cout << endl;

    cout << "    copy_constructible: " << std::is_copy_constructible<T>::value << endl;
    cout << "    trivially_copy_constructible: " << std::is_trivially_copy_constructible<T>::value << endl;
    cout << "    nothrow_copy_constructible: " << std::is_nothrow_copy_constructible<T>::value << endl;

    cout << endl;

    cout << "    copy_assignable: " << std::is_copy_assignable<T>::value << endl;
    cout << "    trivially_copy_assignable: " << std::is_trivially_copy_assignable<T>::value << endl;
    cout << "    nothrow_copy_assignable: " << std::is_nothrow_copy_assignable<T>::value << endl;
 
    cout << endl;

    cout << "    move_constructible: " << std::is_move_constructible<T>::value << endl;
    cout << "    trivially_move_constructible: " << std::is_trivially_move_constructible<T>::value << endl;
    cout << "    nothrow_move_constructible: " << std::is_nothrow_move_constructible<T>::value << endl;

    cout << endl;

    cout << "    move_assignable: " << std::is_move_assignable<T>::value << endl;
    cout << "    trivially_move_assignable: " << std::is_trivially_move_assignable<T>::value << endl;
    cout << "    nothrow_move_assignable: " << std::is_nothrow_move_assignable<T>::value << endl;

    cout << endl;

    cout << "   destructible " << std::is_destructible<T>::value << endl; 
    cout << "   trivially_destructible " << std::is_trivially_destructible<T>::value << endl; 
    cout << "   nothrow_destructible " << std::is_nothrow_destructible<T>::value << endl; 

}

int main4(void)
{
    test_things<Foo>("Foo");
    return 0;
}

int main(void)
{
    {
        try
        {
            // read a JSON file
            std::ifstream i("data\\citylots.json");

            json j;

            high_resolution_clock::time_point t1 = high_resolution_clock::now();
            i >> j;
            high_resolution_clock::time_point t2 = high_resolution_clock::now();
            duration<double> time_span = duration_cast<duration<double>>(t2 - t1);
            cout << "nlohmann parsed in " << time_span.count() << "s" << endl;

        }
        catch (const std::exception & e)
        {
            cout << e.what() << endl;
        }
    }

    {
        high_resolution_clock::time_point t1 = high_resolution_clock::now();
        std::string input = read_file_into_string("data\\citylots.json");
        //const char* c_input = read_entire_file_in_memory("data\\citylots.json");
        //std::string s_input = std::string(c_input);
        //Assert(s_input == input, "ouch!");
        high_resolution_clock::time_point t2 = high_resolution_clock::now();
        duration<double> time_span = duration_cast<duration<double>>(t2 - t1);

        cout << "[mine] file read in " << time_span.count() << "s" << endl;

        t1 = high_resolution_clock::now();
        JSON_Value json = parse_json(input);
        t2 = high_resolution_clock::now();
        time_span = duration_cast<duration<double>>(t2 - t1);

        cout << "[mine] json parsed in " << time_span.count() << "s" << endl;
    }

    

    cout << "done" << endl;
    getchar();

    return 0;
}

int main2(void)
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

    high_resolution_clock::time_point t1 = high_resolution_clock::now();
    input = read_file_into_string("data\\citylots.json");
    high_resolution_clock::time_point t2 = high_resolution_clock::now();
    duration<double> time_span = duration_cast<duration<double>>(t2 - t1);

    cout << "file read in " << time_span.count() << "s" << endl;

    t1 = high_resolution_clock::now();
    JSON_Value json = parse_json(input);
    t2 = high_resolution_clock::now();
    time_span = duration_cast<duration<double>>(t2 - t1);

    cout << "json parsed in " << time_span.count() << "s" << endl;

#if 0
    // read a JSON file
    std::ifstream i("data\\citylots.json");

    try
    {
        json j;
        i >> j;
    }
    catch (const std::exception & e)
    {
        cout << e.what() << endl;
    }
#endif // 0

    getchar();

    return 0;
}
