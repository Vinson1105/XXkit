/**
 2020.06.03
 1、函数增加前缀，取消设置语言后重新创建UI的机制，更改为发出通知‘sigCurrentLanguageChanged’
 
 2019.02.20
 app内语言切换,注意可选语言为当前工程目录下的'.lproj'文件名称列表一致
 */

#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN

@interface NSBundle(Language)
/**
 返回当前可用的语言列表
 */
+ (nullable NSArray*) xx_avaliableLanguages;

/**
 返回当前app内设置的语言,返回nil则表示当前是跟随系统语言
 */
+ (nullable NSString*)xx_currentLanguage;

/**
 设置app语言,注意需要跟'.lproj'文件名称一致
 @param language 语言缩写,nil即为跟随系统
 */
+ (void)xx_setLanguage:(nullable NSString*)language;
@end
NS_ASSUME_NONNULL_END
