//
//  UIView+Popup.h
//  iCamSee
//
//  Created by VINSON on 2020/3/23.
//  Copyright © 2020 Macrovideo. All rights reserved.
//

#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN

@interface UIView(Popup)
@property (nonatomic,strong) NSArray *layoutForPopup;
@property (nonatomic,strong) NSArray *layoutForPopdown;
@property (nonatomic,assign) BOOL popup;
@end

NS_ASSUME_NONNULL_END
