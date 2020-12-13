//
//  UIView+KeyboradAdapter.h
//  XXkitProject
//
//  Created by VINSON on 2020/12/13.
//  Copyright © 2020 郭文轩. All rights reserved.
//

#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN

@interface UIView(KeyboradAdapter)
@property (nonatomic,strong) NSLayoutConstraint *keyboradAdapter_vlayout;
-(void)keyboradAdapter_install:(BOOL)install;
@end

NS_ASSUME_NONNULL_END
