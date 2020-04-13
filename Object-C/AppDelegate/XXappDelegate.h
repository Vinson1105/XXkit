/**
 * 2020-04-11
 * 1、新增创建导航栏接口
 * 2、新增window属性，在创建导航栏时会对此属性初始化，并显示该window
 * 3、实现applicationLaunchFinished函数，但只是单纯返回YES，继承类重载是需要调用super的实现，方便日后扩展
 *
 * 2020-03-25
 *  1、旋转控制，兼容非主线程调用
 *  2、app状态通知，配合【UIViewController+ApplicationState】使用
 */

#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN

@interface XXappDelegate : UIResponder<UIApplicationDelegate>
@property (nonatomic,strong) UIWindow *window;
/**
 * @brief 锁定旋转
 * @param lock YES：锁定，NO：解锁
 */
+ (void)rotationLock:(BOOL)lock;

/**
 * @brief 获取旋转锁定状态
 * @return 返回YES表示已锁定
 */
+ (BOOL)isRotationLocked;

/**
 * @brief 创建一个导航栏
 * @param root 创建导航栏时使用的ViewController
 */
- (void)createNavigationWithRootViewController:(UIViewController*)root;
@end

NS_ASSUME_NONNULL_END
