/**
 * @author: GWX
 * @date: 20190109
 * @descirption: 对UITextField作下拉选择扩展.
 * @history:
 *        20190218 - GWX
 *        1.移除属性willPopupBlock和属性willPackupBlock
 *        2.增加currentKeyChangedBlock,当属性currentKey进行改变时,调用此block
 */

#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN

typedef enum : NSUInteger {
    XXcomboBoxState_OnPopup    = 1,
    XXcomboBoxState_OnPackup   = 2,
    XXcomboBoxState_OnDisabled = 3,
} XXcomboBoxState;

@interface XXcomboBox : NSObject<UITextFieldDelegate>
@property (nonatomic,copy,nullable) NSString *currentKey;           // 当前已选中的item对应的key,nil即为无选中
@property (nonatomic,copy,nullable) void(^currentKeyChangedBlock)(NSString * _Nullable previousKey, NSString * _Nullable currentKey);
@property (nonatomic,assign) NSUInteger popupHeight;                // 弹出框高度,默认为100
@property (nonatomic,assign) NSUInteger itemHeight;                 // 弹出框中Item项的高度,默认为自适应

@property (nonatomic,assign) CGSize indicatorSize;                  // 右边栏指示器大小,默认为textField高度的2/3
@property (nonatomic,assign) CGFloat indicatorRightMargin;          // 右边栏指示器右边距,默认为textField高度的1/3

/**
 * @brief    返回实例
 * @param    textField 对应的textField控件
 * @param    rootView textField所在的页面的顶层view,一般是对应页的UIViewController.view
 * @return   返回实例
 */
- (instancetype) initWithTextField:(UITextField*)textField RootView:(UIView*)rootView;

/**
 * @brief    设置可选项的关键字和文本
 * @param    key 关键字集合
 * @param    text 文本集合
 * @param    currentKey 当前已选中关键字,不触发currentKeyChanged
 */
- (void) setItemKey:(NSArray*)key Text:(NSArray*)text CurrentKey:(NSString* _Nullable)currentKey;

/**
 * @brief    移除所有设置可选项
 */
- (void) removeAllItem;

/**
 * @brief    设置不同状态下的右侧图标
 * @param    image 图片
 * @param    state 对应的状态
 */
- (void) setIndicatorImage:(UIImage*)image WithState:(XXcomboBoxState)state;
@end

NS_ASSUME_NONNULL_END
