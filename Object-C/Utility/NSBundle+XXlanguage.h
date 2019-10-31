/**
 * @author: GWX
 * @date: 20190220
 * @descirption: app内语言切换,注意可选语言为当前工程目录下的'.lproj'文件名称列表一致
 * @history:
 *        date - author
 */

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

@interface NSBundle(XXlanguage)
/**
 * 返回当前可用的语言列表
 */
+ (nullable NSArray*) avaliableLanguages;

/**
 * 返回当前app内设置的语言,返回nil则为跟随系统
 */
+ (nullable NSString*) currentLanguage;

/**
 * 设置app语言,注意需要跟'.lproj'文件名称一致
 *
 * @param    language 语言缩写,nil即为跟随系统
 * @param    refreshable YES:自动刷新View,否则不自动刷新,注意自动刷新只是兼容了一部分的情况,如有特殊情况需要手动刷新UI
 */
+ (void) setCurrentLanguage:(nullable NSString*)language Refresh:(BOOL)refreshable;
@end
