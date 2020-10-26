//
//  QuickLoader.h
//  XXkitProject
//
//  Created by 郭文轩 on 2020/10/12.
//  Copyright © 2020 郭文轩. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN

@interface QuickLoader : NSObject
/**
 根据类名创建UIViewController，会尝试UIStoryboard和一般加载，UIStoryboard仅支持NSBundle.mainBundle。
 */
+(UIViewController*)createViewController:(NSString*)cls;

/**
 设置多UIViewController的Quick配置数据文件。
 @param fileName 文件名称
 @param bundle 文件包
 */
+(void)setMainQuickDataWithFileName:(NSString*)fileName bundle:(nullable NSBundle*)bundle;
+(void)setQuickComponentWithClass:(NSString*)component;

+(BOOL)page:(UIViewController*)obj;
+(BOOL)item:(id)obj data:(id)data;
@end

NS_ASSUME_NONNULL_END
