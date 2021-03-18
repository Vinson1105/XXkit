//
//  XXShadowCreator.h
//  KitDemo
//
//  Created by VINSON on 2018/12/7.
//  Copyright © 2018 VINSON. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN

@interface XXshadowCreator : NSObject

/**
 * @brief    为指定的view创建一个阴影,指定的view必须有superView
 * @param    view 需要创建阴影的目标
 * @param    bgColor 阴影view的背景颜色
 * @param    color 阴影颜色
 * @param    offset 阴影区域的偏移
 * @param    opacity 阴影的透明度,只限于边框区域
 * @param    radius 阴影半径
 */
+ (void) makeShadowWithTarget:(UIView*)view
             ShadowBgColor:(UIColor*)bgColor
               ShadowColor:(UIColor*)color
              ShadowOffset:(CGSize)offset
             ShadowOpacity:(CGFloat)opacity
              ShadowRadius:(CGFloat)radius;
@end

NS_ASSUME_NONNULL_END
