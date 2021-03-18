/**
 2020.04.25 基于XXtextFieldShell将账号登录的基本控制封装组件
 
 基本有以下三种应用场景
 1、使用UITextField自带的删除、外带密码模式切换按键
    ① [shell configPasswordClearButton:nil secureButton:secureButton secureON:secureON secureOFF:secureOFF];
 
 2、不使用UITextField的自带删除，全部外设
    ① [shell configAccountClearButton:clearButton];
    ② [shell configPasswordClearButton:clearButton secureButton:secureButton secureON:secureON secureOFF:secureOFF];
 
 3、不使用UITextField的自带删除，全部重新内设
    ① [shell configClearImage:(UIImage*)clearImage secureON:secureON secureOFF:secureOFF size:size margin:margin spacing:spacing]
 */

#import <UIKit/UIKit.h>
#import "./XXtextFieldShell.h"

NS_ASSUME_NONNULL_BEGIN

@interface XXloginShell : NSObject
@property (nonatomic,strong,readonly) XXtextFieldShell *accountShell;
@property (nonatomic,strong,readonly) XXtextFieldShell *passwordShell;

/**
 配置账号和密码的UITextField
 */
- (void)shellAccount:(UITextField*)account password:(UITextField*)password;

/**
 配置账号和密码UITextField的Logo
 */
- (void)configAccountLogo:(nullable UIImage*)accountLogo passwordLogo:(nullable UIImage*)passwordLogo size:(CGSize)size;

/**
 配置账号的一键删除、密码的一键删除和安全模式的图标
 @param clearImage 账号以及密码的一键删除图标
 @param secureON 密码的安全模式图标（开启），ON和OFF都设置nil则只配置清除按键图标
 @param secureOFF 密码的安全模式图标（关闭）
 @param size 图标大小
 @param margin 图标组的外边距
 @param spacing 图标组之间的间距
 */
- (void)configClearImage:(UIImage*)clearImage
                secureON:(nullable UIImage*)secureON
               secureOFF:(nullable UIImage*)secureOFF
                    size:(CGSize)size
                  margin:(CGFloat)margin
                 spacing:(CGFloat)spacing;

/**
 配置账号一键删除按键
 */
- (void)configAccountClearButton:(UIButton*)accountClear;

/**
 配置密码一键删除、安全模式按键
 */
- (void)configPasswordClearButton:(nullable UIButton*)clearButton
                     secureButton:(UIButton*)secureButton
                         secureON:(UIImage*)secureON
                        secureOFF:(UIImage*)secureOFF;

/**
 配置登录按键
 */
- (void)configOKButton:(UIButton*)button action:(void(^)(NSString *,NSString *))action;
@end

NS_ASSUME_NONNULL_END
