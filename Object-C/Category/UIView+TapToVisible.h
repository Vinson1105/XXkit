/**
 2020.05.25
 1、修复重复点击时，定时器触发重置异常
 
 2019.11.21
 通过点击来显示/隐藏指定的view，当view设置【UIView+Popup】的layout时，会调用popup/popdown，否则只会调用hidden；
 通过设置xx_visibleInterval决定是否启动定时器隐藏view，如①，否则只会②；
    ① 点击 -> 显示并开启（重置）定时器 -> 定时器触发 -> 隐藏
    ② 点击 -> 显示 -> 点击 -> 隐藏
 */

#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN

@interface UIView(TapToVisible)
/** 是否保持当前visible的状态，不被定时器触发和点击影响 */
@property (nonatomic,assign) BOOL xx_holdon;
/** 切换当前visible状态 */
@property (nonatomic,assign) BOOL xx_visible;
/** 目标view列表，可以对其动态修改 */
@property (nonatomic,strong) NSArray *xx_visibleViews;
/** 定时器触发间隔，请在install之前设置，不设置则不启动定时器 */
@property (nonatomic,assign) NSTimeInterval xx_visibleInterval;

/**
 装载、卸载功能，在dealloc中最好进行卸载
 */
- (void)xx_installTapToVisible:(BOOL)install;
@end

NS_ASSUME_NONNULL_END
