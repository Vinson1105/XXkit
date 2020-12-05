//
//  UIViewController+ActivityIndicator.h
//  iCamSee
//
//  Created by VINSON on 2020/11/11.
//  Copyright © 2020 Macrovideo. All rights reserved.
//

#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN

@interface UIViewController(ActivityIndicator)
@property (nonatomic,strong) UIColor *activityIndicator_color;
@property (nonatomic,strong) UIColor *activityIndicator_backgroundColor;
-(void)activityIndicator_show;
-(void)activityIndicator_hide;
@end

NS_ASSUME_NONNULL_END
