/**
 * 2020-04-27
 * 基础提示显示工具
 */
#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@interface XXtoast : NSObject
+ (void)showMessage:(NSString*)message;
+ (void)showMessage:(NSString*)message duration:(CGFloat)duartion;
@end

NS_ASSUME_NONNULL_END
