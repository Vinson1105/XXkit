/**
 2020.04.27
 居中显示背景有透明度的弱提示，默认展示时长1.5s
 */

#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN

@interface XXtoast : NSObject
+ (void)showMessage:(NSString*)message;
+ (void)showMessage:(NSString*)message duration:(NSTimeInterval)duartion;
@end

NS_ASSUME_NONNULL_END
