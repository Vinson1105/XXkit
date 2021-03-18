/**
 2020.05.20
 1、修复重复创建window，初始的ViewController无法释放导致内存泄漏
 
 2020.05.05
 1、简易初始化Navigation接口，使用Scene时，在继承类中重载willConnectToSession函数并对Navigation初始化（详细请看目录下的Readme）
 2、内置Scene状态通知，配合UIViewController+SceneState使用
 */

#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN

@interface XXsceneDelegate : UIResponder <UIWindowSceneDelegate>
@property (nonatomic,strong) UIWindow *window;

/**
 创建一个导航栏
 @param root 创建导航栏时使用的ViewController
 */
- (void)createNavigationWithRootViewController:(UIViewController*)root;

/**
 创建一个导航栏，并指定使用UIStoryboard的初始化初始化入口作为根视图
 @param storyboard 使用的UIStoryboard的文件名
 @param bundle 文件对应的包
 */
- (void)createNavigationWithIntitalViewControllerAtStoryboard:(NSString*)storyboard bundle:(nullable NSBundle*)bundle;
@end

NS_ASSUME_NONNULL_END
