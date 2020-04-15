/**
 * 2020-04-14
 * 1、新增创建根视图接口
 *
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

/**
 * @brief 创建一个导航栏，并以指定的类名创建根视图
 * @param root 导航栏的根视图类名
 * @param type 视图的加载方式，有UIStoryboard和Xib两种
 * @param bundle 文件对应的包
 */
- (void)createNavigationWithRootViewController:(NSString*)root type:(NSString*)type bundle:(nullable NSBundle*)bundle;

/**
 * @brief 创建一个导航栏，并指定使用UIStoryboard的初始化初始化入口作为根视图
 * @param storyboard 使用的UIStoryboard的文件名
 * @param bundle 文件对应的包
 */
- (void)createNavigationWithUIStoryboard:(NSString*)storyboard bundle:(nullable NSBundle*)bundle;

/**
 * @brief 创建一个导航栏，并指定使用nib文件作为根视图
 * @param bundle 文件对应的包
 */
- (void)createNavigationWithNibClass:(NSString*)nib bundle:(nullable NSBundle*)bundle;
@end

NS_ASSUME_NONNULL_END
