//
//  ButtonShell.h
//  iCamSee
//
//  Created by VINSON on 2019/11/19.
//  Copyright © 2019 Macrovideo. All rights reserved.
//

/**
 2019.11.19
 button的互斥选择组件
 */

#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN


@interface XXbuttonExclusiveShell : NSObject
/** 当前选中，等于nil时，表示没有选中任何button */
@property (nonatomic,copy,readonly) NSString *selected;
/** 选中改变回调 */
@property (nonatomic,copy) void(^onSelectChanged)(XXbuttonExclusiveShell *shell, NSString * _Nullable previous, NSString * _Nullable current);

/**
 添加button，name和button需要一一对应，如果已经存在name对应的button，则将会覆盖原来的button
 @param button 需要添加的button
 @param name 对应button的关键字
 */
- (void)addButton:(UIButton*)button name:(NSString*)name;

/**
 移除button
 @param name 对应button的关键字
 */
- (void)remove:(NSString*)name;

/**
 移除所有button
 */
- (void)removeAll;

/**
 重置当前选中值，如果name没有对应的button，则重置无效，注意：此函数不会触发【onSelectChanged】
 @param name 对应button的关键字
 */
- (void)reset:(NSString*)name;

/**
 重置当前选中值，如果name没有对应的button，则重置无效，注意：此函数会触发【onSelectChanged】
 @param name 对应button的关键字
 */
- (void)select:(NSString*)name;

/**
 查询name对应的button，没有对应的button时，返回nil
 @param name 对应button的关键字
 */
- (nullable UIButton*)button:(NSString*)name;
@end

NS_ASSUME_NONNULL_END
