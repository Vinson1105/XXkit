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

+(void)setMainFile:(NSString*)mainFile;
+(BOOL)item:(id)obj data:(id)data;
+(void)installComponentClass:(NSString*)component;
@end

NS_ASSUME_NONNULL_END
