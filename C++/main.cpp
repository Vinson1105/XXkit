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
    // mapRef(0) = 0;
    // mapRef(1) = 1;
    // mapRef(2) = 2;
    // mapRef(3) = 3;
    mapRef["AAA"] = "aaa";
    for(int i = 0; i < 2; i++){
        mapRef["Class"]["Student"](i)["Name"]    = std::string("Name") + std::to_string(i);
        mapRef["Class"]["Student"](i)["ID"]      = i;

        mapRef["Class"]["Student"](i)["ParamDict\"STR\""]["A"] = "ParamA";
        mapRef["Class"]["Student"](i)["ParamDict\"STR\""]["B"] = "ParamB";
        mapRef["Class"]["Student"](i)["ParamDict\"STR\""]["C"] = "ParamC";
        mapRef["Class"]["Student"](i)["ParamArr\"STR\""](0)    = "ParamA";
        mapRef["Class"]["Student"](i)["ParamArr\"STR\""](1)    = "ParamB";
        mapRef["Class"]["Student"](i)["ParamArr\"STR\""](2)    = "ParamC";

        mapRef["Class"]["Student"](i)["ParamDict\"INT\""]["A"] = 1;
        mapRef["Class"]["Student"](i)["ParamDict\"INT\""]["B"] = 2;
        mapRef["Class"]["Student"](i)["ParamDict\"INT\""]["C"] = 3;
        mapRef["Class"]["Student"](i)["ParamArr\"INT\""](0)    = 11;
        mapRef["Class"]["Student"](i)["ParamArr\"INT\""](1)    = 22;
        mapRef["Class"]["Student"](i)["ParamArr\"INT\""](2)    = 33;
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

    XXmap map3;
    XXmapRef map3Ref(map3);
    map3Ref["\"a\""] = "a";
    //map3Ref["b"] = "bbbb";

    XXmapRef target = map3;

    XXjson json;
    json.fromMap(target);

    cout << "XXmap(Source):" << endl;
    for (auto iter = map.data().begin(); iter != map.data().end(); iter++){
        std::cout << iter->first << " = " << iter->second << std::endl;
    }
    cout << endl;
    
    cout << "Jmap:" << endl;
    for (auto iter = json.data().begin(); iter != json.data().end(); iter++){
        std::cout << iter->first << " = " << iter->second << std::endl;
    }
    cout << endl;

    std::string jsonString = json.toString(false, false);
    cout << "JsonString:" << endl;
    cout << jsonString << endl;
    cout << endl;

    jsonString = json.toString(true, false);
    cout << "JsonString(Thin):" << endl;
    cout << jsonString << endl;
    cout << endl;

    XXmap tomap;
    XXmapRef tomapRef(tomap);
    json.toMap(tomapRef);
    cout << "XXmap(ToMap):" << endl;
    for (auto iter = tomap.data().begin(); iter != tomap.data().end(); iter++){
        std::cout << iter->first << " = " << iter->second << std::endl;
    }
    cout << endl;

    json.fromString(jsonString, false);
    cout << "Jmap(FromString):" << endl;
    for (auto iter = json.data().begin(); iter != json.data().end(); iter++){
        std::cout << iter->first << " = " << iter->second << std::endl;
    }
    cout << endl;

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