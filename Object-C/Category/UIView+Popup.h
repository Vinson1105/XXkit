/**
 2020.03.23
 UIView的简单动效弹出、收起
 */

#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN

@interface UIView(Popup)
@property (nonatomic,strong) NSArray *xx_popupConstraint;
@property (nonatomic,strong) NSArray *xx_popdownConstraint;
@property (nonatomic,assign) BOOL xx_popup;
@end

NS_ASSUME_NONNULL_END
