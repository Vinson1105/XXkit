//
//  XXloopButtonShell.h
//  iCamSee
//
//  Created by VINSON on 2020/5/11.
//  Copyright © 2020 Macrovideo. All rights reserved.
//

/**
 2020.05.11
 button的循环点击，每一次点击触发切换下一个配置的image，并触发onClicked，当在配置列表中的最后一个进行点击时，切换到第一个
 */

#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN

@interface XXbuttonCycleClickShell : NSObject
@property (nonatomic,weak,readonly) UIButton *button;
@property (nonatomic,copy) void(^onClicked)(XXbuttonCycleClickShell *shell, NSString *previous, NSString *current);

/**
 配置目标button
 */
- (void)shell:(UIButton*)button;

/**
 增加循环的配置，第一个添加的会被设置为当前选中
 @param key 关键字，是这个image（配置）在一整个循环中的标识，当前激活到这个image时，会以这个key通过回调返回
 @param image button的图片
 */
- (void)add:(NSString*)key image:(UIImage*)image;

/**
 重置当前选中
 @param key 需要显示配置的key
 */
- (void)reset:(NSString*)key;
@end

NS_ASSUME_NONNULL_END
