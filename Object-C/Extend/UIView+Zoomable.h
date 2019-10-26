/**
 * @author: GWX
 * @date: 20191026
 * @descirption: 以缩放移动前的view.frame作为边界限制,对view进行缩放以及移动
 */

#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN

@interface UIView (Zoomable)
@property (nonatomic,assign) BOOL zoomable;     // 默认:NO;
@property (nonatomic,assign) CGFloat scaleMax;  // 最大缩放比例,需要大于1.0,默认:4.0
@property (nonatomic,assign) CGFloat scaleMin;  // 最小缩放比例,需要大于0并小于1.0,默认:1.0

/** 恢复缩放移动 */
- (void) zoomRestore;
@end

NS_ASSUME_NONNULL_END
