//
//  QuickComponentBase.h
//  XXkitProject
//
//  Created by 郭文轩 on 2020/10/14.
//  Copyright © 2020 郭文轩. All rights reserved.
//

#import <Foundation/Foundation.h>

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
