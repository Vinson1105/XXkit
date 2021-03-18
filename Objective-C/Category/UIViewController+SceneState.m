#import "UIViewController+SceneState.h"
#import <objc/runtime.h>

#define kSceneDelegate                  @"sceneDelegate"
#define kSigSceneDidDisconnect          @"sigSenceDidDisconnect"
#define kSigSceneWillResignActive       @"sigSenceWillResignActive"
#define kSigSceneDidEnterBackground     @"sigSenceDidEnterBackground"
#define kSigSceneDidBecomeActive        @"sigSenceDidBecomeActive"
#define kSigSceneWillEnterForeground    @"sigSenceWillEnterForeground"

#define kOnSceneStateChanged            @"onSceneStateChanged"

@implementation UIViewController(SceneState)
- (void)setOnSceneStateChanged:(void (^)(SceneState, XXsceneDelegate * _Nonnull))onSceneStateChanged{
    objc_setAssociatedObject(self, kOnSceneStateChanged, onSceneStateChanged, OBJC_ASSOCIATION_COPY);
}
- (void (^)(SceneState, XXsceneDelegate * _Nonnull))onSceneStateChanged{
    return objc_getAssociatedObject(self, kOnSceneStateChanged);
}

-(void)installSceneState:(BOOL)install{
    if(install){
        [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(onSceneDidDisconnect:) name:kSigSceneDidDisconnect object:nil];       // 已经断开
        [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(onSceneWillResignActive:) name:kSigSceneWillResignActive object:nil];       // 即将进入后台
        [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(onSceneDidEnterBackground:) name:kSigSceneDidEnterBackground object:nil];   // 已经进入后台
        [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(onSceneWillEnterForeground:) name:kSigSceneWillEnterForeground object:nil]; // 即将进入前台
        [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(onSceneDidBecomeActive:) name:kSigSceneDidBecomeActive object:nil];         // 已经进入后台
    }
    else{
        [[NSNotificationCenter defaultCenter] removeObserver:self name:kSigSceneDidDisconnect object:nil];
        [[NSNotificationCenter defaultCenter] removeObserver:self name:kSigSceneWillResignActive object:nil];
        [[NSNotificationCenter defaultCenter] removeObserver:self name:kSigSceneDidEnterBackground object:nil];
        [[NSNotificationCenter defaultCenter] removeObserver:self name:kSigSceneWillEnterForeground object:nil];
        [[NSNotificationCenter defaultCenter] removeObserver:self name:kSigSceneDidBecomeActive object:nil];
    }
}

-(void)onSceneDidDisconnect:(NSNotification*)notifition{
    if(self.onSceneStateChanged){
        NSDictionary *userInfo = notifition.userInfo;
        self.onSceneStateChanged(SceneState_didDisconnect, userInfo[kSceneDelegate]);
    }
}
-(void)onSceneDidBecomeActive:(NSNotification*)notifition{
    if(self.onSceneStateChanged){
        NSDictionary *userInfo = notifition.userInfo;
        self.onSceneStateChanged(SceneState_didBecomeActive, userInfo[kSceneDelegate]);
    }
}
-(void)onSceneWillEnterForeground:(NSNotification*)notifition{
    if(self.onSceneStateChanged){
        NSDictionary *userInfo = notifition.userInfo;
        self.onSceneStateChanged(SceneState_willEnterForeground, userInfo[kSceneDelegate]);
    }
}
-(void)onSceneWillResignActive:(NSNotification*)notifition{
    if(self.onSceneStateChanged){
        NSDictionary *userInfo = notifition.userInfo;
        self.onSceneStateChanged(SceneState_willResignActive, userInfo[kSceneDelegate]);
    }
}
-(void)onSceneDidEnterBackground:(NSNotification*)notifition{
    if(self.onSceneStateChanged){
        NSDictionary *userInfo = notifition.userInfo;
        self.onSceneStateChanged(SceneState_didEnterBackground, userInfo[kSceneDelegate]);
    }
}
@end
