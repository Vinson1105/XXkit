## XXmap、XXmapRef
1. 一个以路径key存放value的Map<String,String>，定位为数据量小，扁平化，提供便捷并有限的数据读写；
2. 非线程安全，性能方面有待提升，优化点：getRealPath、std::string预分配、容器类key/value指针化；
3. [key]中的key不允许为'.'，(index)中的index为数组下标；

## XXjson、XXjvalue
1. 将JSON
1. 暂时开发XXmap转换到JSON文本接口
2. key1/key2 = "12" 和 key1/key2/key3 = "123"类似此两组键值并存时，并不符合JSON语法，可能导致数据掉失，暂时未作其他转换

> XXmap map;   
> XXmapRef mapRef(map);  
> mapRef["Class"](0)["Name"] = "ClassA";  
> mapRef["Class"](1)["Name"] = "ClassB";  
> mapRef["Class"](2)["Name"] = "ClassC";
