#import "UIViewController+ApplicationState.h"
#import <objc/runtime.h>

#define kSigApplicationWillResignActive     @"sigApplicationWillResignActive"
#define kSigApplicationDidEnterBackground   @"sigApplicationDidEnterBackground"
#define kSigApplicationDidBecomeActive      @"sigApplicationDidBecomeActive"
#define kSigApplicationWillEnterForeground  @"sigApplicationWillEnterForeground"

#define kOnApplicationStateChanged            @"onApplicationStateChanged"

@implementation UIViewController(ApplicationState)
- (void)setOnApplicationStateChanged:(void (^)(ApplicationState))onApplicationStateChanged{
    objc_setAssociatedObject(self, kOnApplicationStateChanged, onApplicationStateChanged, OBJC_ASSOCIATION_COPY);
}
- (void (^)(ApplicationState))onApplicationStateChanged{
    return objc_getAssociatedObject(self, kOnApplicationStateChanged);
}

-(void)installApplicationState:(BOOL)install{
    if(install){
        [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(onApplicationWillResignActive:) name:kSigApplicationWillResignActive object:nil];       // 即将进入后台
        [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(onApplicationDidEnterBackground:) name:kSigApplicationDidEnterBackground object:nil];   // 已经进入后台
        [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(onApplicationWillEnterForeground:) name:kSigApplicationWillEnterForeground object:nil]; // 即将进入前台
        [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(onApplicationDidBecomeActive:) name:kSigApplicationDidBecomeActive object:nil];         // 已经进入后台
    }
    else{
        [[NSNotificationCenter defaultCenter] removeObserver:self name:kSigApplicationWillResignActive object:nil];
        [[NSNotificationCenter defaultCenter] removeObserver:self name:kSigApplicationDidEnterBackground object:nil];
        [[NSNotificationCenter defaultCenter] removeObserver:self name:kSigApplicationWillEnterForeground object:nil];
        [[NSNotificationCenter defaultCenter] removeObserver:self name:kSigApplicationDidBecomeActive object:nil];
    }
}
-(void)onApplicationDidBecomeActive:(NSNotification*)notifition{
    if(self.onApplicationStateChanged){
        self.onApplicationStateChanged(ApplicationState_didBecomeActive);
    }
}
-(void)onApplicationWillEnterForeground:(NSNotification*)notifition{
    if(self.onApplicationStateChanged){
        self.onApplicationStateChanged(ApplicationState_willEnterForeground);
    }
}
-(void)onApplicationWillResignActive:(NSNotification*)notifition{
    if(self.onApplicationStateChanged){
        self.onApplicationStateChanged(ApplicationState_willResignActive);
    }
}
-(void)onApplicationDidEnterBackground:(NSNotification*)notifition{
    if(self.onApplicationStateChanged){
        self.onApplicationStateChanged(ApplicationState_didEnterBackground);
    }
}
@end
