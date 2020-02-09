#include <string>
#include <vector>
#include <array>

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

enum class JSON_Type : u32
{
    Unknown, Object, Array, String, Number, True, False, Null 
};


struct JSON_Value
{
    JSON_Type type = JSON_Type::Unknown;
    union
    {
        JSON_Object object;
        JSON_Array array;
        std::string string;
        double num;
        bool boolean;
    };
};

int main()
{
    
    return 0;
}