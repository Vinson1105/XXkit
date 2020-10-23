//
//  UIViewController+SafeLoad.h
//  XXkitProject
//
//  Created by 郭文轩 on 2020/10/23.
//  Copyright © 2020 郭文轩. All rights reserved.
//

#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN

@interface UIViewController(SafeLoad)
+(UIViewController*)safeLoad_init;
@end

NS_ASSUME_NONNULL_END
