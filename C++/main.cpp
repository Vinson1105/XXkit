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
    for(int i = 0; i < 500; i++){
        mapRef["Class"]["Student"](i)["Name"]    = std::string("Name") + std::to_string(i);
        mapRef["Class"]["Student"](i)["ID"]      = i;
        mapRef["Class"]["Student"](i)["ParamDictionary"]["A"]   = "ParamA";
        mapRef["Class"]["Student"](i)["ParamDictionary"]["B"]   = "ParamB";
        mapRef["Class"]["Student"](i)["ParamDictionary"]["C"]   = "ParamC";
        mapRef["Class"]["Student"](i)["ParamArray"](0)          = "ParamA";
        mapRef["Class"]["Student"](i)["ParamArray"](1)          = "ParamB";
        mapRef["Class"]["Student"](i)["ParamArray"](2)          = "ParamC";
    }
    mapRef["ZZZ"] = "ZZZ";

    XXmap map2;
    XXmapRef map2Ref(map2);
    map2Ref["Array"](0)["Name"] = "Name1";
    map2Ref["Array"](0)["ID"] = "ID1";
    map2Ref["Array"](0)["ParamDictionary"]["ParamA"] = "ParamA";
    map2Ref["Array"](0)["ParamDictionary"]["ParamB"] = "ParamB";
    map2Ref["Array"](0)["ParamArray"](0) = "ParamA";
    map2Ref["Array"](0)["ParamArray"](1) = "ParamB";
    map2Ref["Array"](1)["Name"] = "Name2";
    map2Ref["Array"](1)["ID"] = "ID2";
    map2Ref["Array"](2)["Name"] = "Name3";
    map2Ref["Array"](2)["ID"] = "ID3";

    XXjson json;
    json.fromMap(mapRef);

    // cout << "XXmap:" << endl;
    // for (auto iter = map.data().begin(); iter != map.data().end(); iter++){
    //     std::cout << iter->first << " = " << iter->second << std::endl;
    // }
    
    // cout << "Jmap:" << endl;
    // for (auto iter = json.data().begin(); iter != json.data().end(); iter++){
    //     std::cout << iter->first << " = " << iter->second << std::endl;
    // }

    std::string jsonString = json.toString();
    cout << "JsonString:" << endl;
    cout << jsonString << endl;

    jsonString = json.toString(true);
    cout << "JsonString(Thin):" << endl;
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