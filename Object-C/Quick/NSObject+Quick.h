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
 NSObject.quick_class需要是匹配的Component.targetClass的基类或者同类。优先匹配同类，其次匹配其基类。
 NSObject.quick_class缺省时，使用NSObject.class。
 
 component1.targetClass{
    return NSStringFromClass(UILabel.class);
 }
 component2.targetClass{
    return NSStringFromClass(UIView.class);
 }
 component3.targetClass{
    return NSStringFromClass(CustomLabel.class);
 }
 
 CustomLabel.quick_class = @"UILabel"，则匹配到component1，其次是component2；
 CustomLabel.quick_class = nil，则匹配到component3，若是派生自UILabel则其次是component1，若是派生自UIView则其次是component2；
 */
@property (nonatomic,copy) NSString *quick_class;

/**
 保存自身的子对象，一般是在UIViewController才有这个SubObject
 */
@property (nonatomic,strong,readonly) NSDictionary *quick_nameToSubObject;

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

/**
 添加子对象，obj需要具有有效的quick_name
 */
-(void)quick_addSubObject:(id)obj;

/**
 移除子对象，obj需要具有有效的quick_name
 */
-(void)quick_removeSubObject:(id)obj;

/**
 移除子对象
 */
-(void)quick_removeSubObjectWithName:(NSString*)name;
@end

NS_ASSUME_NONNULL_END
