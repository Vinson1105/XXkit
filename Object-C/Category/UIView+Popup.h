/**
 2020.03.23
 UIView的简单动效弹出、收起
 */

#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN

@interface UIView(Popup)
@property (nonatomic,strong) NSArray *popup_upLayoutConstraints;
@property (nonatomic,strong) NSArray *popup_downLayoutConstraints;

@property (nonatomic,copy) void(^popup_blockWhenWillUp)(void);
@property (nonatomic,copy) void(^popup_blockWhenWillDown)(void);

@property (nonatomic,copy) void(^popup_blockWhenUping)(void);
@property (nonatomic,copy) void(^popup_blockWhenDowning)(void);

@property (nonatomic,copy) void(^popup_blockWhenDidUpFinished)(void);
@property (nonatomic,copy) void(^popup_blockWhenDidDownFinished)(void);

@property (nonatomic,assign) BOOL popup_up;
@property (nonatomic,assign) BOOL popup_active;

-(void)popup_active:(BOOL)active animate:(BOOL)animate;
@end

NS_ASSUME_NONNULL_END
