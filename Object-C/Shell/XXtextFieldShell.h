/**
 2020.07.08
 1、增加属性maxLength，用于限制可输入长度
 2、增加属性strongMatchMode，强制匹配，根据正则表达式只允许匹配的字符串输入（注意：当前有长度区间的表达式谨慎使用）
 
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
@property (nonatomic,readonly,strong) UITextField *target;
/** 正则表达式 */
@property (nonatomic,copy,nullable) NSString *expression;
/** UITextField内容是否符合正则表达式；若正则表达式为nil，一直为NO；UITextField内容为空时，为NO */
@property (nonatomic,assign,readonly) BOOL isMatching;
/** 强制匹配模式，开启之后，需要满足正则表达式才会更改内容，不满足则会一直无法输入，默认为NO */
@property (nonatomic,assign) BOOL strongMatchMode;
/** 长度限制，与expression是并行对text进行限制，因为设置expression之后并关闭强制匹配模式，expression有一定长度需求时，无法对长度限制（默认为0，不作限制） */
@property (nonatomic,assign) NSInteger maxLength;

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
