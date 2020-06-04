/**
 2019.11.19
 页切换组件，具备边界使能控制
 */

#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN

@interface XXbuttonPageShiftShell : NSObject
@property (nonatomic,weak,readonly) UIButton *prevButton;
@property (nonatomic,weak,readonly) UIButton *nextButton;
@property (nonatomic,weak,readonly) UILabel *pageLabel;
@property (nonatomic,copy,nullable) void(^onPageTouchChanged)(XXbuttonPageShiftShell *shell, int previous, int current);

@property(nonatomic,assign,readonly) int current;       // 当前页
@property(nonatomic,assign,readonly) int count;         // 页数量
@property (nonatomic,copy) NSString *format;            // 页标签的显示格式，默认为'%d/%d'，如共有8页，当前位于第5页，则显示'5/8'

/**
 组件初始化
 */
- (void)shellWithPrev:(UIButton*)prev next:(UIButton*)next page:(nullable UILabel*)page;

/**
 重置当前页和数量，组件初始化之后，需要调用一次本函数，以初始化当前页和数量
 */
- (void)resetCurrent:(int)current count:(int)count;
@end

NS_ASSUME_NONNULL_END
