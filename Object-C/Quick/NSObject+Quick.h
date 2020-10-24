//
//  NSObject+Quick.h
//  XXkitProject
//
//  Created by 郭文轩 on 2020/10/12.
//  Copyright © 2020 郭文轩. All rights reserved.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@interface NSObject(Quick)
/**
 标识对象的名称，用于在‘QuickData’中定位对应的数据块。
 同一个VC下的不同层级对象的数据块在‘QuickData’是同一层级，所以对于这个‘quick_name’在同一个VC下原则上不支持存在相同的。
 quick_name:{
    key:value
    key:value
    key:value
 }
 */
@property (nonatomic,copy) NSString *quick_name;

/**
 标识对象的使用‘QuickComponent’，根据此值，选择匹配的‘QuickComponent.targetClass’进行加载数据。
 缺省时，将根据对象的类型，优先匹配相等的‘QuickComponent.targetClass’，再去匹配‘isKindOfClass’。
 component1.targetClass{
    return NSStringFromClass(UILabel.class);
 }
 component2.targetClass{
    return NSStringFromClass(UIView.class);
 }
 component3.targetClass{
    return NSStringFromClass(UIButton.class);
 }
 
 CustomLabel.quick_class = @"UILabel"，则匹配到component1；
 CustomLabel.quick_class = nil，则优先匹配到component1，如果没有则匹配到component2；
 */
@property (nonatomic,copy) NSString *quick_class;

/**
 使用QuickData进行重置配置
 */
-(void)quick_reset:(id)data;

/**
 读取文件中的JSON数据并用之进行Quick配置；
 @param file 目标JSON文件的名称；
 @param bundle 文件所在的bundle，默认是用[NSBundle mainBundle]；
 */
-(void)quick_resetFromJFile:(NSString*)file bundle:(nullable NSBundle*)bundle;
@end

NS_ASSUME_NONNULL_END
