#include <iostream>
#include "./XXmap/XXmap.h"
#include <map>
#include <vector>
#include "./XXmap/XXjson.h"
#include "./Common/XXstdStringExtend.h"

typedef struct StructA{
    int a;
    int b;
    int c;
    char d[32];
}StructA;
typedef struct StructB{
    StructA array[32];
}StructB;


using namespace std;
int main(){
    StructB structB;
    int size = sizeof(structB);

    XXmap map;
    XXmapRef mapRef(map);
    mapRef["AAA"] = "AAA";
    mapRef["Class"]["Student"](0)["Name"]    = "A";
    mapRef["Class"]["Student"](0)["ID"]      = 1;

    mapRef["Class"]["Student"](1)["Name"]    = "B";
    mapRef["Class"]["Student"](1)["ID"]      = 2;

    mapRef["Class"]["Student"](2)["Name"]    = "C";
    mapRef["Class"]["Student"](2)["ID"]      = 3;

    mapRef["Class"]["Student"](3)["Name"]    = "D";
    mapRef["Class"]["Student"](3)["ID"]      = 4;

    mapRef["Class"]["Student"](4)["Name"]    = "E";
    mapRef["Class"]["Student"](4)["ID"]      = 5;
    mapRef["Class"]["Student"](4)["Param"]["A"]  = "ParamA";
    mapRef["Class"]["Student"](4)["Param"]["B"]  = "ParamB";
    mapRef["Class"]["Student"](4)["Param"]["C"]  = "ParamC";

    mapRef["Class"]["hahaha"] = "hahaha";

    // mapRef["Student"](5)["Name"]    = "F";
    // mapRef["Student"](5)["ID"]      = 6;
    // mapRef["Student"](5)["Param"](0)  = "Param1";
    // mapRef["Student"](5)["Param"](1)  = "Param2";
    // mapRef["Student"](5)["Param"](2)  = "Param3";

    mapRef["ZZZ"](0) = "Array1";
    mapRef["ZZZ"](1) = "Array2";
    mapRef["ZZZ"](2) = "Array3";

    XXmap map2;
    XXmapRef map2Ref(map2);
    map2Ref["Array"](0) = "Array1";
    map2Ref["Array"](1) = "Array2";
    map2Ref["Array"](2) = "Array3";

    XXjson json;
    json.fromMap(mapRef);

    cout << "XXmap:" << endl;
    for (auto iter = map.data().begin(); iter != map.data().end(); iter++){
        std::cout << iter->first << " = " << iter->second << std::endl;
    }
    
    cout << "Jmap:" << endl;
    for (auto iter = json.data().begin(); iter != json.data().end(); iter++){
        std::cout << iter->first << " = " << iter->second << std::endl;
    }

    cout << "JsonString:" << endl;
    std::string jsonString = json.toString();
    cout << jsonString << endl;

    cout << "JsonString(Thin):" << endl;
    jsonString = json.toString(true);
    cout << jsonString << endl;

    // std::string csv = "forename,middlename,surname,phone";
    // std::string path = "/usr/local/bin/myapp"; // First field is empty
    // std::string data = "forename**middlename**surname**phone";

    // cout << XXstdStringExtend::section(csv, ",", 2, 2) << endl;     // str == "surname"
    // cout << XXstdStringExtend::section(path, "/", 3, 4) << endl;    // str == "bin/myapp"
    // cout << XXstdStringExtend::section(path, "/", -1) << endl;      // str == "myapp"
    // cout << XXstdStringExtend::section(path, "/", 0,1) << endl;      // str == "/usr"
    // cout << XXstdStringExtend::section(path, "/", 3, 3) << endl ;   // str == "myapp"
    // cout << XXstdStringExtend::section(data, "**", 2, 2) << endl;   // str == "surname"
    // cout << XXstdStringExtend::section(data, "**", -3, -2) << endl; // str == "middlename**surname"
    return 0; 
}