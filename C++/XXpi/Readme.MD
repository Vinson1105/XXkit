----------------------------------------------------------------------
# XXenterPi - 一种以字符串形式、在相邻键值对中以‘\r\n’作为分隔符的基础通信格式
XXenterPi:SubPi\r\n		**SubPi为使用的实际协议**
key1:value1\r\n
key2:value2\r\n
key3:value3\r\n
...
\r\n

#XXlogPi - 日志通信协议，是XXenterPi的子协议
XXenterPi:XXlogPi\r\n
Type:xxx\r\n			**日志的类型:Normal、Warning、Error**
Source:xxx\r\n			**日志的来源:该日志产生的源头**
Timestamp:xxx\r\n   	**日志产生的时间戳(GTM+0)**    
Message:xxx\r\n
\r\n


----------------------------------------------------------------------
# XXbytePi - 一种以字节形式、按不同的数据以不同的字节数存储的通信格式，
#           并且这种格式可以使用一个JSON数据表来描述不同的数据，是使用哪种格式存储
XXbytePi + LENGTH(64) + ...
{
    ByteOrder:Big/Little,
    Context:[
        **方式1，注意name不能包含’_‘字符**
        "name_type_start_Length",  
        **方式2**
        {
            name:,      **该值的名称**
            type:,      **Int（64/32/16/8）、String**
            start:,     **该值的起始位置**
            Length:,    **该值的字节长度，Int:可以省缺，String:不可省缺**
        },
    ]
    
}