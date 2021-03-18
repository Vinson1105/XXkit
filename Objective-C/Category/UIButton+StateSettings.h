#import <UIKit/UIKit.h>
NS_ASSUME_NONNULL_BEGIN

@interface UIButton(StateSettings)
- (void)setBorderColor:(UIColor*)color forState:(UIControlState)state;
- (void)setBackgroundColor:(UIColor*)color forState:(UIControlState)state;
@end

NS_ASSUME_NONNULL_END
