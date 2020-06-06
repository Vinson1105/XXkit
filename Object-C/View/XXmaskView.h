/**
 2020.06.06
 1、增加单例接口，对代码重新整理，移除在hidden状态下，对content的强引用（想要使用content快捷显示maskview，可以使用UIView+MaskView）
 
 2018.10.23
 1、在单例模式下,如果新的content与上一个相同,则沿用上一个的content和hideHandler
 2、 默认背景是0.5Alpha,如果需要实现透明功能需要初始化一个,不建议直接修改单例
 */

#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN

@interface XXmaskView : UIView<UIGestureRecognizerDelegate>
/** 单例 */
+ (XXmaskView*)sharedInstance;

/**
 居中展示内容，通过hidden隐藏，隐藏之后会异常对view的相关引用
 @param view 需要展示的内容
 @param animated 动效（暂时没有实现）
 */
- (void)show:(UIView*)view animated:(BOOL)animated;
@end

NS_ASSUME_NONNULL_END
