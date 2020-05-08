/**
 2020.04.23
 1、删除配置左右图标接口，更改为只能配置左边图标
 2、右边固定为一键清空和安全模式功能
 3、默认当前编辑中就会显示清除按键
 
 2020.04.22  对UITextField的简单封装
 1、正则表达式实时判断
 2、使用image简单配置left view或者right view
 */

#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN

@interface XXtextFieldShell : NSObject<UITextFieldDelegate>
/** 目标UITextField */
@property (nonatomic,readonly,weak) UITextField *target;
/** 正则表达式 */
@property (nonatomic,copy,nullable) NSString *expression;
/** UITextField的text改变 */
@property (nonatomic,copy,nullable) void(^onTextChanged)(XXtextFieldShell *shell, NSString *text);
/** UITextField的开始编辑 */
@property (nonatomic,copy,nullable) void(^onEditBegin)(XXtextFieldShell *shell);
/** UITextField的结束编辑 */
@property (nonatomic,copy,nullable) void(^onEditEnd)(XXtextFieldShell *shell);

/**
 设置UITextField
 */
- (void)shell:(UITextField*)target;

/**
 配置左边的图标
 */
- (void)configLogo:(UIImage*)image size:(CGSize)size;

/**
 配置右边的一键清除按键，编辑中并UITextField.text不为空时显示
 @param image 按键图标
 @param size 按键大小
 @param margin 按键的外边距
 */
- (void)configClear:(UIImage*)image size:(CGSize)size margin:(CGFloat)margin;

/**
 配置右边的一键清除和安全模式按键，一键清除按键在编辑中并UITextField.text不为空时显示，安全模式按键为总是显示
 @param clear 一键清除按键图标
 @param secureON 安全模式按键图标（开启）
 @param secureOFF 安全模式按键图标（关闭）
 @param size 按键大小
 @param margin 按键的外边距（与背景框的距离）
 @param spacing 按键之间的距离
 */
- (void)configClear:(UIImage*)clear
           secureON:(UIImage*)secureON
          secureOFF:(UIImage*)secureOFF
               size:(CGSize)size
             margin:(CGFloat)margin
            spacing:(CGFloat)spacing;
@end

NS_ASSUME_NONNULL_END
