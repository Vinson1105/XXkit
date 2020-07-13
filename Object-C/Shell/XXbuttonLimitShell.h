/**
 2020.07.08
 1、增加公开属性‘enabled’，私有属性‘limited’；
    其中‘limited’定义为：当前点击后一段时间内禁止点击的状态；
    ‘enabled’则是非limited状态下，button的目标的使能状态，当通过shell设置button是，‘enabled’的值会同步当前button.enabled；
    ‘limited’的优先级是大于‘enabled’;
 2、增加‘limited’状态下自定义倒计时格式‘format’，当‘format’为空时，不会显示倒计时，默认为“%ds”
 
 2020.04.29
 点击限制按键封装，一般应用场景是验证码获取后，若干秒不能点击，并动态显示剩余时间
 */

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN

@interface XXbuttonLimitShell : NSObject
@property (nonatomic,strong,readonly) UIButton *button; // 目标button
@property (nonatomic,assign) BOOL enabled;              // 使能按键，不等于limit，limit的优先级大于enabled
@property (nonatomic,copy) NSString *format;            // 限制时倒计时的显示格式，如："%ds重新获取"，其中‘%d’是必要字符
@property (nonatomic,assign) BOOL clickEnterLimit;      // 是否点击后进入限制模式，默认：YES
@property (nonatomic,assign) BOOL limited;              // 指示按键时处于限制状态，可以手动开启关闭
@property (nonatomic,assign) int duration;              // 限制时长，单位秒，默认：60s

@property (nonatomic,copy) void(^onClicked)(XXbuttonLimitShell *shell); // button点击回调

/**
 配置目标按键
 */
- (void)shell:(UIButton*)button;
@end

NS_ASSUME_NONNULL_END
