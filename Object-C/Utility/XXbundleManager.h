//
//  XXbundleManager.h
//  XXkitDemo
//
//  Created by VINSON on 2019/10/30.
//  Copyright © 2019 VINSON. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN

@interface XXbundleManager : NSObject
+(nullable NSBundle*) bundleWithName:(NSString*)name;
+(nullable NSBundle*) bundleWithName:(NSString*)name type:(NSString*)type;
+(nullable UIImage*) image:(NSString*)imageName inBundle:(nullable NSString*)bundleName;
+(nullable UINib*) nib:(NSString*)nibName inBundle:(nullable NSString*)bundleName;
+(nullable UIStoryboard*) storyboard:(NSString*)storyboardName inBundle:(nullable NSString*)bundleName;
@end

NS_ASSUME_NONNULL_END
