#import "NSBundle+XXlanguage.h"
#import <objc/runtime.h>
#define STANDARD_USER_DEFAULT   [NSUserDefaults standardUserDefaults]
static NSString *languageKey                = @"XXlanguageKey"; // 语言存储的关键字

//
//
//
@interface XXlanguageBundle : NSBundle
@end

@implementation XXlanguageBundle
+ (NSBundle *)mainBundle{
    // 获取之前存储的语言并加载翻译文件
    NSString *currentLanguage = [STANDARD_USER_DEFAULT valueForKey:languageKey];
    if(nil != currentLanguage){
        NSString *path = [[super mainBundle] pathForResource:currentLanguage ofType:@"lproj"];
        if (path.length) {
            return [NSBundle bundleWithPath:path];
        }
    }
    return nil;
}
- (NSString *)localizedStringForKey:(NSString *)key value:(NSString *)value table:(NSString *)tableName
{
    // 返回mainBundle,是根据当前语言,返回mainBundle为nil时,即为语言是跟随系统
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
+ (NSString*) currentLanguage{
    return [STANDARD_USER_DEFAULT valueForKey:languageKey];
}
+ (void) setCurrentLanguage:(NSString* _Nullable)language Refresh:(BOOL)refreshable{
    static NSString *lastLanguage = nil;
    lastLanguage = [STANDARD_USER_DEFAULT valueForKey:languageKey];
    if (language == lastLanguage) {
        return;
    }
    
    if (nil == language) {
        [STANDARD_USER_DEFAULT removeObjectForKey:languageKey];
    }
    else{
        [STANDARD_USER_DEFAULT setValue:language forKey:languageKey];
    }
    
    if (refreshable) {
        dispatch_async(dispatch_get_main_queue(), ^{
            [self refreshViewController];
        });
    }
}

+ (void)load{
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        // 替换原来的mainBundle
        object_setClass([NSBundle mainBundle], [XXlanguageBundle class]);
    });
}

+ (void) refreshViewController{
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
