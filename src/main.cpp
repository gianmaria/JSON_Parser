#include <stdio.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <exception>
#include <chrono>

using namespace std::chrono;
using std::cout;
using std::endl;


#include "nlohmann/json.hpp"
using nlohmann::json;

#include "rapidjson/document.h"
using namespace rapidjson;


#define GB_JSON_PARSER_IMPL
#include "gb_json_parser.h"

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

std::string read_file_into_string(const std::string &filename)
{
    std::ifstream ifs(filename, std::ifstream::binary);

    std::stringstream buffer;
    buffer << ifs.rdbuf();

    return buffer.str();
}

char* read_entire_file_in_memory(const char* file_path)
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

int main(void)
{
    test_things<JSON_Token>("JSON_Token");
    return 0;
    const char* input = read_entire_file_in_memory("data\\citylots.json");

    if (!input)
    {
        cout << "Cannot read json file :(" << endl;
        return 1;
    }

#if 0
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
#endif // 0


    {
        high_resolution_clock::time_point t1 = high_resolution_clock::now();
        JSON_Value json = parse_json(input);
        high_resolution_clock::time_point t2 = high_resolution_clock::now();
        milliseconds time_span = duration_cast<milliseconds>(t2 - t1);

        cout << "[mine] json parsed in " << time_span.count() << "ms" << endl;

        if (json.type == JSON_Type_Object)
        {
            cout << "obj size is: " << json.object->data.size() << endl;
            if (json.object->data[2].key == "features" &&
                json.object->data[2].value.type == JSON_Type_Array)
            {
                cout << "features array size: " << json.object->data[2].value.array->data.size() << endl;
                int stop = 0;
            }
        }
    }


#if 0
    {
        Document document;
        high_resolution_clock::time_point t1 = high_resolution_clock::now();
        //document.Parse<kParseIterativeFlag>(input);
        high_resolution_clock::time_point t2 = high_resolution_clock::now();
        seconds time_span_s = duration_cast<seconds>(t2 - t1);
        milliseconds time_span_ms = duration_cast<milliseconds>(t2 - t1);

        cout << "[rapidjson iterative] json parsed in " << time_span_s.count() << "s " << time_span_ms.count() << "ms" << endl;
    }

    {
        Document document;
        high_resolution_clock::time_point t1 = high_resolution_clock::now();
        document.Parse(input);
        high_resolution_clock::time_point t2 = high_resolution_clock::now();
        seconds time_span_s = duration_cast<seconds>(t2 - t1);
        milliseconds time_span_ms = duration_cast<milliseconds>(t2 - t1);

        cout << "[rapidjson recursive] json parsed in " << time_span_s.count() << "s " << time_span_ms.count() << "ms" << endl;

        if (document.IsObject())
        {
            for (auto& m : document.GetObject())
                printf("Member %s\n", m.name.GetString());
        }

        if (document.HasMember("features"))
        {
            auto& features = document["features"];

            if (features.IsArray())
            {
                cout << "size: " << features.Size() << endl;
            }

            for (auto& v : features.GetArray())
            {
                if (v.IsObject())
                {
                    const auto& feature = v.GetObject();
                    cout << "member count: " << feature.MemberCount() << endl;
                }
            }

        }
    }
#endif // 0


    cout << "done" << endl;
    //getchar();

    return 0;
}
