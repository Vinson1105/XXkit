//
//  QuickComponentBase.h
//  XXkitProject
//
//  Created by 郭文轩 on 2020/10/14.
//  Copyright © 2020 郭文轩. All rights reserved.
//

#import <Foundation/Foundation.h>

#define CONST_STR(name) static NSString * const k##name = @ #name;
#define IS_KEY_MATCH(str) [key isEqualToString:str]
#define IS_VALUE_KIND(cls) [value isKindOfClass:cls.class]

/**
 对于这个模式还需要考虑一下，先使用比较愚蠢比较繁琐方式先实现，再以此为基础总结升华。
 初步总有以下问题：
 1、没有一个标准化输入输出
 当初没有使用标准化是，不打算一般使用时有一些键值捆绑到业务层，但是在现在看起来，使用q模式需要有一套标准输入出
 */
//#define UsingQuickPMInterface
#ifdef UsingQuickPMInterface
    #define QUICK_PROPERTY(pname,ptype) \
    +(NSDictionary*)q_##pname{ \
        return @{ \
            @"name":@#pname, \
            @"type":@#ptype, \
            @"call":@"property", \
        }; \
    }

    #define QUICK_METHOD(mname,...)\
    +(NSDictionary*)q_##mname{ \
        return @{ \
            @"name":@#mname, \
            @"args":@#__VA_ARGS__, \
            @"call":@"method", \
        }; \
    }

    #define QUICK_SIGNAL(sname,...)\
    +(NSDictionary*)q_##sname{ \
        return @{ \
            @"name":@#sname, \
            @"args":@#__VA_ARGS__, \
            @"call":@"signal", \
        }; \
    }

    #define QUICK_ACTION(aname,...)\
    +(NSDictionary*)q_##aname{ \
        return @{ \
            @"name":@#aname, \
            @"args":@#__VA_ARGS__, \
            @"call":@"method", \
        }; \
    }

#else
    #define QUICK_PROPERTY(pname,ptype)
    #define QUICK_METHOD(mname,...)
    #define QUICK_SIGNAL(sname,...)
    #define QUICK_ACTION(aname,...)
#endif

NS_ASSUME_NONNULL_BEGIN

@protocol QuickComponentDelegate <NSObject>
+(NSString*)targetClass;
+(BOOL)quick:(id)obj kvdata:(NSDictionary*)kvdate;
@end

@interface QuickComponentBase : NSObject
/**
 标识Component对应的类型
 +(NSString *)targetClass{
    return NSStringFromClass( UIxxx.class );
 }
 */
+(NSString *)targetClass;

/**
 遍历类型实例的配置数据，每一项都会调用此函数进行配置，继承类必须重载此函数；
 函数内慎用[NSObject quick_reset]，尤其不能在Component对应的类型使用上使用quick_reset，否则可能会死循环；
 
 if( [key isEqualToString:@"xxx"] ){
    obj.xxx = value;
 }
 else if( [key isEqualToString:@"yyy"] ){
    if( [value isKindOfClass:NSString.class] ){
        obj.yyy = value;
    }
    else{
        [self unexecutedKey: value:];   // 如果value并不是预期的类型，也可以调用这个归一化的函数提示一下
    }
 }
 else {
    [super obj: key: value: ];      // 缺省时，调用super的函数，可以用于提示未处理的key-value
 }
 
 @param obj 此Component对应的类型实例
 @param key 此配置项key
 @param value 此配置项value
 */
+(void)obj:(id)obj key:(NSString*)key value:(id)value;

/**
 没有处理的key归一函数
 */
+(void)unexecutedKey:(NSString*)key value:(id)value;
@end

NS_ASSUME_NONNULL_END
