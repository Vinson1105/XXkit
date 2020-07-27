//
//  XXpickerViewShell.h
//  iCamSee
//
//  Created by VINSON on 2020/7/21.
//  Copyright © 2020 Macrovideo. All rights reserved.
//
/**
 2020.07.21
 1、使用默认row显示的简单封装
 */

#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN

@interface XXpickerViewShell : NSObject
@property (nonatomic,copy) void(^onSelectChanged)(XXpickerViewShell *shell, NSArray *selecteds);

/** 目标UIPickerView */
@property (nonatomic,strong,readonly) UIPickerView *pickerView;
/** PickerView中行高，默认：30 */
@property (nonatomic,assign) int rowHeight;
/** PickerView中unselected的字体颜色，默认gray */
@property (nonatomic,strong) UIColor *unselectedColor;
/** PickerView中selected的字体颜色，默认blue */
@property (nonatomic,strong) UIColor *selectedColor;

/**
 设置目标UIPickerView
 */
- (void)shell:(UIPickerView*)pickerView;

/**
 设置可选项
 @param data 可选项列表
 @param component 组件编号
 */
- (void)setData:(NSArray*)data atComponent:(int)component;
//- (void)setTreeData:(NSArray*)data;
- (void)removeDataAtComponent:(int)component;
- (void)selectString:(NSString*)string atComponent:(int)component;
- (void)selectString:(NSArray<NSString*>*)string;
@end

NS_ASSUME_NONNULL_END
