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
 * @brief    返回当前app语言
 * @return   nil:跟随系统
 */
+ (NSString*) currentLanguage;

/**
 * @brief    设置app语言,注意需要跟'.lproj'文件名称一致
 * @param    language 语言缩写,nil即为跟随系统
 * @param    refreshable YES:自动刷新View,否则不自动刷新,注意自动刷新只是兼容了一部分的情况,如有特殊情况需要手动刷新UI
 */
+ (void) setCurrentLanguage:(NSString* _Nullable)language Refresh:(BOOL)refreshable;
@end
