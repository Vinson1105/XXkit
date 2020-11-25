//
//  XXmodalView.h
//  teset
//
//  Created by VINSON on 2020/9/29.
//  Copyright © 2020 XX. All rights reserved.
//

#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN

@interface XXmodalView : UIView
@property (nonatomic,assign) BOOL popup;
@property (nonatomic,assign) BOOL touchToPopdown;
@property (nonatomic,assign) BOOL backgroundColorTransparent;
@property (nonatomic,strong,readonly) UIView *contentView;

-(void)configContentView:(UIView*)contentView popupConstraint:(NSArray*)popup popdownConstraint:(NSArray*)popdown;
-(void)configContentViewAtCenter:(UIView*)contentView size:(CGSize)size margin:(CGFloat)margin;
-(void)configContentViewAtBottom:(UIView*)contentView size:(CGSize)size margin:(CGFloat)margin;
@end

NS_ASSUME_NONNULL_END
