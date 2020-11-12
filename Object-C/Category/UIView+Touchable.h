//
//  UIView+Touchable.h
//  XXkitProject
//
//  Created by VINSON on 2020/11/12.
//  Copyright © 2020 郭文轩. All rights reserved.
//

#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN

@interface UIView(Touchable)
-(void)touchable_install:(void(^)(void))handler;
@end

NS_ASSUME_NONNULL_END
