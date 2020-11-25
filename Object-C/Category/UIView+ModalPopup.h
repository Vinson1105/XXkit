//
//  UIView+ModalPopup.h
//  XXkitProject
//
//  Created by 郭文轩 on 2020/11/16.
//  Copyright © 2020 郭文轩. All rights reserved.
//

#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN

@interface UIView(ModalPopup)
@property (nonatomic,assign) BOOL modalPopup_popup;
@property (nonatomic,assign) BOOL modalPopup_touchBackgroundToPopdown;
@property (nonatomic,assign) BOOL modalPopup_backgroundColorTransparent;
@property (nonatomic,strong,readonly) UIView *modalPopup_backgroundView;

-(void)modalPopup_configPopupConstraint:(NSArray*)popup popdownConstraint:(NSArray*)popdown;
-(void)modalPopup_configAtCenterSize:(CGSize)size margin:(CGFloat)margin;
-(void)modalPopup_configAtBottomSize:(CGSize)size margin:(CGFloat)margin;
-(void)modalPopup_release;  // 注意：释放view时需要手动调用release
@end

NS_ASSUME_NONNULL_END
