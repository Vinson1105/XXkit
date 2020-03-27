/**
 * 2020-03-25
 *  1、旋转控制，兼容非主线程调用
 *  2、app状态通知，配合【UIViewController+ApplicationState】使用
 */

#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN

@interface XXappDelegate : UIResponder<UIApplicationDelegate>
+ (void)rotationLock:(BOOL)lock;
+ (BOOL)isRotationLocked;
@end

NS_ASSUME_NONNULL_END
