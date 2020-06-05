/*
    说明:
    20181023
    1. 在单例模式下,如果新的content与上一个相同,则沿用上一个的content和hideHandler
    2. 默认背景是0.5Alpha,如果需要实现透明功能需要初始化一个,不建议直接修改单例
    20181208
    1. 从继承UIView更改为继承UIObject,将蒙层view私有化,避免继承uiview时加入到应用窗口中导致无法正确释放的问题
*/

#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN

@interface XXmaskView : UIView<UIGestureRecognizerDelegate>
/// 方案一
@property (nonatomic,assign) BOOL touchToHidden;
@property (nonatomic,assign) BOOL animated;
+ (XXmaskView*)maskViewWithContent:(UIView*)content;
+ (XXmaskView*)maskViewWithContentAtCenter:(UIView*)content edgeInsets:(UIEdgeInsets)edgeInsets;
+ (XXmaskView*)maskViewWithContentAtBottom:(UIView*)content edgeInsets:(UIEdgeInsets)edgeInsets;

/// 方案二
+ (XXmaskView*)sharedInstance;
- (void)show:(UIView*)view animated:(BOOL)animated;

/**
 * @brief    单例,使用单例时,在某些情况下可能出现无法显示的问题,暂时没有解决
 */
//+ (instancetype) sharedInstance;

/**
 * @brief    初始化
 * @param    content 在蒙层中显示的内容
 * @param    willHideBlock 蒙层将要隐藏时执行的回调,返回YES隐藏,否则不隐藏
 * @param    alpha 蒙层的透明度,默认为0.5
 */
- (instancetype) initWithContent:(UIView*)content
                    WillHideBlock:(nullable BOOL(^)(void))willHideBlock
                            Alpha:(CGFloat)alpha;

/**
 * @brief    显示并设置蒙层中内容,此函数配合单例使用
 * @param    content 在蒙层中显示的内容
 * @param    willHideBlock 蒙层将要隐藏时执行的回调,返回YES隐藏,否则不隐藏
 */
- (void) show:(UIView* )content WillHideBlock:(BOOL(^ __nullable)(void))willHideBlock;

/**
 * @brief   显示
 */
- (void) show;

/**
 * @brief   隐藏
 */
- (void) hide;

/**
 * @brief   判断蒙层是否隐藏
 * @return  返回YES,隐藏中
 */
//- (BOOL) isHidden;
@end

NS_ASSUME_NONNULL_END
