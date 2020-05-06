/**
 2020-05-06
 1、调整横竖屏状态获取
 2、新增旋转对应的layout接口
 注意：
 1、UIApplicationWillChangeStatusBarOrientationNotification/UIApplicationWillChangeStatusBarOrientationNotification相关在iOS13之后会被废弃，后续需要及时更新
 2、UIApplicationWillChangeStatusBarOrientationNotification存在触发两次的情况，后续需要及时修复
 
 2019-10-14 UIViewController的横屏竖控制
 */

#import <UIKit/UIKit.h>
NS_ASSUME_NONNULL_BEGIN

@interface UIViewController (Orientation)
@property (nonatomic,assign,readonly) BOOL isLandscape;
@property (nonatomic,copy,nullable) void(^onOrientationWillChange)(void);
@property (nonatomic,copy,nullable) void(^onOrientationDidChanged)(void);
@property (nonatomic,strong,nullable) NSArray<NSLayoutConstraint*> *portraitLayouts;
@property (nonatomic,strong,nullable) NSArray<NSLayoutConstraint*> *landscapeLayouts;

- (void)installOrientation:(BOOL)install;
- (void)landscapeLeft;
- (void)landscapeRight;
- (void)portrait;
@end

NS_ASSUME_NONNULL_END
