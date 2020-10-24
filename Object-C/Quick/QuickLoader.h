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
+(UIViewController*)createViewController:(NSString*)cls;

+(void)setMainQuickDataWithFileName:(NSString*)fileName bundle:(nullable NSBundle*)bundle;
+(void)setQuickComponentWithClass:(NSString*)component;

+(BOOL)page:(UIViewController*)obj;
+(BOOL)item:(id)obj data:(id)data;
@end

NS_ASSUME_NONNULL_END
