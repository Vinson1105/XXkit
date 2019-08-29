#XXenterPi - 一种以字符串形式、在相邻键值对中以‘\r\n’作为分隔符的基础通信格式
XXenterPi:SubPi\r\n		**SubPi为使用的实际协议**
key1:value1\r\n
key2:value2\r\n
key3:value3\r\n

#XXlogPi - 日志通信协议
XXenterPi:XXlogPi\r\n
Type:xxx\r\n			**日志的类型:Normal、Warning、Error**
Source:xxx\r\n			**日志的来源:该日志产生的源头**
Timestamp:xxx\r\n   	**日志产生的时间戳(GTM+0)**    
Message:xxx\r\n


