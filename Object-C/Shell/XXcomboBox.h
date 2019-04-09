//
//  XXComboBox.h
//
//
//  Created by VINSON on 2018/11/16.
//  Copyright © 2018 Macrovideo. All rights reserved.
//

#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN

typedef enum : NSUInteger {
    XXComboBoxPopupState = 1,
    XXComboBoxPackupState = 2,
    XXComboBoxDisabledState = 3,
} XXComboBoxState;

@interface XXcomboBox : NSObject<UITextFieldDelegate>

/// 方法
/**
 * @brief    返回实例
 * @param    textField 对应的textField控件
 * @param    rootView textField所在的页面的顶层view,一般是对应页的UIViewController.view
 * @return   返回实例
 */
- (instancetype) initWithTextField:(UITextField*)textField RootView:(UIView*)rootView;

/**
 * @brief    设置弹出界面的高度,以及当中的每一个可选项的高度
 * @param    popupHeight 弹出界面高度
 * @param    itemHeight 可选项高度
 */
- (void) setPopupHeight:(NSUInteger)popupHeight ItemHeight:(NSUInteger)itemHeight;  

/**
 * @brief    设置可选项的关键字和文本
 * @param    key 关键字集合
 * @param    text 文本集合
 */
- (void) setItemKey:(NSArray*)key Text:(NSArray*)text;

/**
 * @brief    设置不同状态下的右侧图标
 * @param    image 图片
 * @param    state 对应的状态
 */
- (void) setIndicatorImage:(UIImage*)image WithState:(XXComboBoxState)state;

/// 属性
/** 当前已选中的item对应的key,nil即为无选中 */
@property (copy,nonatomic,nullable) NSString *currentKey;
/** 弹出前block */
@property (copy,nonatomic,nullable) void(^willPopupBlock)(void);
/** 收起前block */
@property (copy,nonatomic,nullable) void(^willPackupBlock)(void);
@end

NS_ASSUME_NONNULL_END
