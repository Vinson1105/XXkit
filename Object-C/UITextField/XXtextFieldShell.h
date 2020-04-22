/**
 * 2020-04-22  对UITextField的简单封装
 * 1、正则表达式实时判断
 * 2、使用image简单配置left view或者right view
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
 * @brief 设置UITextField
 */
- (void)shell:(UITextField*)target;

/**
 * @brief 使用image配置UITextField的leftView或者rightView
 */
- (void)configImage:(UIImage*)image mode:(UITextFieldViewMode)mode left:(BOOL)left;
@end

NS_ASSUME_NONNULL_END
