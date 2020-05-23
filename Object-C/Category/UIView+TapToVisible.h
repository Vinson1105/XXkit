//
//  UIViewController+TopTitle.h
//  iCamSee
//
//  Created by VINSON on 2019/11/21.
//  Copyright © 2019 Macrovideo. All rights reserved.
//

#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN

@interface UIView(TapToVisible)
@property (nonatomic,assign) BOOL xx_holdon;
@property (nonatomic,assign) BOOL xx_visible;
@property (nonatomic,strong) NSArray *xx_visibleViews;

- (void)xx_installTapToVisible:(BOOL)install;
@end

NS_ASSUME_NONNULL_END
