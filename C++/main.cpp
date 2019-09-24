#include <iostream>
#include "./XXmap/XXmap.h"
#include <map>
#include <vector>
#include "./XXmap/XXjson.h"

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
    mapRef["Student"](0)["Name"]    = "A";
    mapRef["Student"](0)["ID"]      = 1;

    mapRef["Student"](1)["Name"]    = "B";
    mapRef["Student"](1)["ID"]      = 2;

    mapRef["Student"](2)["Name"]    = "C";
    mapRef["Student"](2)["ID"]      = 3;

    mapRef["Student"](3)["Name"]    = "D";
    mapRef["Student"](3)["ID"]      = 4;

    XXjson json;
    json.fromMap(mapRef);

    cout << "XXmap" << endl;
    for (auto iter = map.data().begin(); iter != map.data().end(); iter++){
        std::cout << iter->first << " = " << iter->second << std::endl;
    }
    
    cout << "Jmap" << endl;
    for (auto iter = json.data().begin(); iter != json.data().end(); iter++){
        std::cout << iter->first << " = " << iter->second << std::endl;
    }
    return 0; 
}