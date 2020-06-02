#import "NSBundle+Language.h"
#import <objc/runtime.h>
#define kLanguage @"Language"
#define kUserDefaults   [NSUserDefaults standardUserDefaults]
#define kSigCurrentLanguageChanged @"sigCurrentLanguageChanged"

//
//
//
@interface XXlanguageBundle : NSBundle
@end

@implementation XXlanguageBundle
+ (NSBundle *)mainBundle{
    /// 根据当前设置的语言，选择对应的包
    NSString *currentLanguage = [kUserDefaults valueForKey:kLanguage];
    if(nil != currentLanguage){
        NSString *path = [[super mainBundle] pathForResource:currentLanguage ofType:@"lproj"];
        if (path.length) {
            return [NSBundle bundleWithPath:path];
        }
    }
    return nil;
}
- (NSString *)localizedStringForKey:(NSString *)key value:(NSString *)value table:(NSString *)tableName{
    /// 返回mainBundle,是根据当前语言,返回mainBundle为nil时,即为语言是跟随系统
    if ([XXlanguageBundle mainBundle]) {
        return [[XXlanguageBundle mainBundle] localizedStringForKey:key value:value table:tableName];
    } else {
        return [super localizedStringForKey:key value:value table:tableName];
    }
}
@end


//
//
//
@implementation NSBundle(XXlanguage)
+ (void)load{
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        /// 替换原来的mainBundle
        object_setClass([NSBundle mainBundle], [XXlanguageBundle class]);
    });
}
+ (NSArray *)xx_avaliableLanguages{
    NSString *dir = [NSBundle mainBundle].resourcePath;
    dir = [dir stringByAppendingString:@""];
    return [[NSBundle mainBundle] pathsForResourcesOfType:@"lproj" inDirectory:dir];
}
+ (nullable NSString*) xx_currentLanguage{
    return [kUserDefaults valueForKey:kLanguage];
}
+ (void)xx_setLanguage:(nullable NSString *)language{
    /// 如果当前language和目标language都是为nil，或者相等直接返回
    NSString *currentLanguage = [self xx_currentLanguage];
    if (currentLanguage==language || [[self xx_currentLanguage] isEqualToString:language]) {
        return;
    }
    
    /// TODO: 判断是否为可选语言
    
    /// 更改语言
    if (nil == language) {
        [kUserDefaults removeObjectForKey:kLanguage];
    }
    else{
        [kUserDefaults setValue:language forKey:kLanguage];
    }
    
    [[NSNotificationCenter defaultCenter] postNotificationName:kSigCurrentLanguageChanged object:nil userInfo:nil];
}

+ (void)refreshViewController{
    // 注意:可是为keyWindow.rootViewController必然为UINavigationController
    UIViewController *root = [UIApplication sharedApplication].keyWindow.rootViewController;
    if(![root isKindOfClass: [UINavigationController class]]){
        return;
    }
    
    UINavigationController *rootNavgation           = (UINavigationController*)root;    // 根UINavigationController
    NSArray<UIViewController*>* newViewControllers  = rootNavgation.viewControllers;    // 根以下的ViewController
    rootNavgation                                   = nil;

    for (UIViewController *vc in newViewControllers) {
        UIViewController *newViewController = [self makeViewController:vc];
        if(nil == newViewController)
            return;
        
        // 初始化新的根UINavigationController,或者把新的ViewControllery压入栈
        if (nil == rootNavgation) {
            rootNavgation = [[UINavigationController alloc] initWithRootViewController:newViewController];
        }
        else{
            [rootNavgation pushViewController:newViewController animated:NO];
        }
        
        // 如果中间层是TabBar需要对selectIndex属性进行初始化
        if ([newViewController isKindOfClass:[UITabBarController class]]) {
            ((UITabBarController*)newViewController).selectedIndex = ((UITabBarController*)vc).selectedIndex;
        }
    }
    
    [UIApplication sharedApplication].keyWindow.rootViewController = rootNavgation;
}
+ (__kindof UIViewController*) makeViewController:(UIViewController *)target{
    UIViewController *ret       = nil;
    NSString *targetClassString = NSStringFromClass([target class]);

    // Storyboard的加载方式
    UIStoryboard *storyboard = target.storyboard;
    if (nil != storyboard) {
        ret = [storyboard instantiateViewControllerWithIdentifier:targetClassString];
    }

    // Nib的加载方式
    if (nil == ret) {
        ret = [[UIViewController alloc] initWithNibName:targetClassString bundle:nil];
    }

    // 默认的加载方式(纯代码)
    if(nil == ret){
        ret = [[[target class] alloc] init];
    }
    
    return ret;
}
@end
