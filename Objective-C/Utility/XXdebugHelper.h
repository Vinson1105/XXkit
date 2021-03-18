//
//  XXdebugHelper.h
//  XXkitProject
//
//  Created by VINSON on 2020/11/12.
//  Copyright © 2020 郭文轩. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN

@interface XXdebugHelper : NSObject
/**
 将调试小组件初始化在哪个window上，可以动态切换window；
 */
+(void)install:(UIWindow*)window;

/**
 设置可配置的属性；
 type的可选值，字符‘=’为分割字符，暂不支持非分割时使用
    - 字符串：string=默认值（aaa）
    - 整型：int=默认值（100）
    - 浮点：float=默认值（0.01）
    - 布尔：bool=默认值（0、1）
    - 单选：[可选项1,可选项2,可选项3]=默认值（可选项2）
    - 多选：[可选项1,可选项2,可选项3]*=默认值（可选项1,可选项2）
 例如
 string=AAA
 int=16
 float=0.01
 bool=0
 [A,B,C,D]=B
 [A,B,C,D]*=A,B
 其中默认是都是可以缺省的
 
 当属性更改时会发出信号，sigDebugPropertyChanged(@{name:xxx,property:xxx})
 */
+(void)addPropertyName:(NSString*)name type:(NSString*)type title:(nullable NSString*)title atBranch:(nullable NSString*)branch;
+(nullable id)getPropertyWithName:(NSString*)name atBranch:(nullable NSString*)branch;
@end

NS_ASSUME_NONNULL_END
