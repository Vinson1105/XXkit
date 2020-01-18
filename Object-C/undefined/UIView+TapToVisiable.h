//
//  UIViewController+TopTitle.h
//  iCamSee
//
//  Created by VINSON on 2019/11/21.
//  Copyright © 2019 Macrovideo. All rights reserved.
//

#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN

@interface UIView(TapToVisiable)
@property (nonatomic,strong) NSArray<UIView*>* views;
@property (nonatomic,assign) BOOL viewsHidden;
@property (nonatomic,assign) BOOL holdon;
-(void)installTapToVisiable:(BOOL)install;
@end

NS_ASSUME_NONNULL_END
