//
//  UIViewController+Keyboard.h
//  XXkitProject
//
//  Created by VINSON on 2020/11/3.
//  Copyright © 2020 郭文轩. All rights reserved.
//

#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN

@interface UIViewController(Keyboard)
/**
 是否开启触碰隐藏键盘
 */
@property (nonatomic,assign) BOOL keyboard_tapToHidden;

/**
 添加需要处理第一响应者的控件，用于当触发需要隐藏键盘时注册为第一响应者
 */
-(void)keyboard_addFirstResponder:(id)responder;

/**
 移除需要处理第一响应者的控件
 */
-(void)keyboard_removeFirstResponder:(id)responder;
@end

NS_ASSUME_NONNULL_END
