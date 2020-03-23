#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN

typedef enum : NSUInteger {
    MaskDisplayPositionCenter,
    MaskDisplayPositionBottom,
} MaskDisplayPosition;

@interface MaskDisplay : NSObject
@property (nonatomic,strong,readonly) UIView *maskView;
@property (nonatomic,assign) BOOL hidden;
//+(void)show:(UIView*)content alpha:(CGFloat)alpha;

-(instancetype)initWithContent:(UIView*)content padding:(CGFloat)padding alpha:(CGFloat)alpha;
//-(instancetype)initWithContent:(UIView*)content;
-(instancetype)initWithCenterContent:(UIView*)content;
-(void)show;
-(void)hide;
@end

NS_ASSUME_NONNULL_END
