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


#if 0
#include "nlohmann/json.hpp"
using nlohmann::json;
#endif // 0

#if 1
#include "rapidjson/document.h"
using namespace rapidjson;
#endif // 0



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
    //test_things<JSON_Token>("JSON_Token");
    //return 0;

    const char* citylots = read_entire_file_in_memory("data\\citylots_txt.txt");

    if (!citylots)
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


#if 1
    {
        std::string test_input;
        test_input = R"FOO("C:\\Windows\\System32 \/root\/user\/bin \"some folder\\illegal\/path\n\" \b \f \n \r \t \uABCD ciao")FOO";
        test_input = R"FOO(123.56e3)FOO";
        test_input = R"FOO([ "hello" , "world" , 123 ,    45.67,  1,2,3 , true,false,   null   ])FOO";
        test_input = R"FOO(
                     {
                       "firstName": "John",
                       "lastName": "Smith",
                       "isAlive": true,
                       "age": 27,
                       "address": {
                         "streetAddress": "21 2nd Street",
                         "city": "New York",
                         "state": "NY",
                         "postalCode": "10021-3100"
                       },
                       "phoneNumbers": [
                         {
                           "type": "home",
                           "number": "212 555-1234"
                         },
                         {
                           "type": "office",
                           "number": "646 555-4567"
                         },
                         {
                           "type": "mobile",
                           "number": "123 456-7890"
                         }
                       ],
                       "children": [],
                       "spouse": null
                     }
        )FOO";

        test_input = R"FOO(

{
    "omega" : "\u03A9",
    "type": "FeatureCollection",
    "features": [
        { "type": "Feature", "properties": { "MAPBLKLOT": "0001001", "BLKLOT": "0001001", "BLOCK_NUM": "0001", "LOT_NUM": "001", "FROM_ST": "0", "TO_ST": "0", "STREET": "UNKNOWN", "ST_TYPE": null, "ODD_EVEN": "E" }, "geometry": { "type": "Polygon", "coordinates": [ [ [ -122.422003528252475, 37.808480096967251, 0.0 ], [ -122.422076013325281, 37.808835019815085, 0.0 ], [ -122.421102174348633, 37.808803534992904, 0.0 ], [ -122.421062569067274, 37.808601056818148, 0.0 ], [ -122.422003528252475, 37.808480096967251, 0.0 ] ] ] } }
        ,
        { "type": "Feature", "properties": { "MAPBLKLOT": "0002001", "BLKLOT": "0002001", "BLOCK_NUM": "0002", "LOT_NUM": "001", "FROM_ST": "0", "TO_ST": "0", "STREET": "UNKNOWN", "ST_TYPE": null, "ODD_EVEN": "E" }, "geometry": { "type": "Polygon", "coordinates": [ [ [ -122.42082593937107, 37.808631474146033, 0.0 ], [ -122.420858049679694, 37.808795641369592, 0.0 ], [ -122.419811958704301, 37.808761809714007, 0.0 ], [ -122.42082593937107, 37.808631474146033, 0.0 ] ] ] } }
        ,
        { "type": "Feature", "properties": { "MAPBLKLOT": "0004002", "BLKLOT": "0004002", "BLOCK_NUM": "0004", "LOT_NUM": "002", "FROM_ST": "0", "TO_ST": "0", "STREET": "UNKNOWN", "ST_TYPE": null, "ODD_EVEN": "E" }, "geometry": { "type": "Polygon", "coordinates": [ [ [ -122.415701204606876, 37.808327252671461, 0.0 ], [ -122.415760743593196, 37.808630700240904, 0.0 ], [ -122.413787891332404, 37.808566801319841, 0.0 ], [ -122.415701204606876, 37.808327252671461, 0.0 ] ] ] } }
    ]
}

)FOO";


        high_resolution_clock::time_point t1 = high_resolution_clock::now();

        const char *input = test_input.c_str();
        input = citylots;

        JSON_Value json;
        parse_json(input, &json);

        high_resolution_clock::time_point t2 = high_resolution_clock::now();
        milliseconds time_span = duration_cast<milliseconds>(t2 - t1);
        cout << "[mine] json parsed in " << time_span.count() << "ms" << endl;

        //if (json.type == JSON_Type_Object)
        //{
        //    cout << "obj size is: " << json.object->data.size() << endl;
        //    if (json.object->data[2].key == "features" &&
        //        json.object->data[2].value.type == JSON_Type_Array)
        //    {
        //        cout << "features array size: " << json.object->data[2].value.array->data.size() << endl;
        //        int stop = 0;
        //    }
        //}
    }
#endif // 0



#if 0
    {
        const char *input = citylots;

        Document document;
        high_resolution_clock::time_point t1 = high_resolution_clock::now();
        
        document.Parse<kParseIterativeFlag>(input);
        
        high_resolution_clock::time_point t2 = high_resolution_clock::now();
        seconds time_span_s = duration_cast<seconds>(t2 - t1);
        milliseconds time_span_ms = duration_cast<milliseconds>(t2 - t1);

        cout << "[rapidjson iterative] json parsed in " << time_span_s.count() << "s " << time_span_ms.count() << "ms" << endl;
    }

    {
        const char *input = citylots;

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

            //for (auto& v : features.GetArray())
            //{
            //    if (v.IsObject())
            //    {
            //        const auto& feature = v.GetObject();
            //        cout << "member count: " << feature.MemberCount() << endl;
            //    }
            //}

        }
    }
#endif // 0


    cout << "done" << endl;
    //getchar();

    return 0;
}
