#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN

@interface TextFieldExpressShell : NSObject<UITextFieldDelegate>
@property (nonatomic,readonly,weak) UITextField *target;
@property (nonatomic,copy,nullable) NSString *express;
-(void)shell:(UITextField*)target;
@end

NS_ASSUME_NONNULL_END
