/**
 2020-04-29 点击限制按键封装，一般应用场景是验证码获取后，若干秒不能点击，并动态显示剩余时间
 */

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN

@interface XXlimitButtonShell : NSObject
@property (nonatomic,strong,readonly) UIButton *button;

/**
 配置目标按键
 */
- (void)shell:(UIButton*)button;

/**
 配置点击后禁能时长
 @param duration 点击后禁能时长，以s为单位，默认60s
 */
- (void)configDuration:(int)duration;

/**
 配置按键的字体颜色
 */
- (void)configNormalColor:(UIColor*)normalColor limitColor:(UIColor*)limitColor;
@end

NS_ASSUME_NONNULL_END
