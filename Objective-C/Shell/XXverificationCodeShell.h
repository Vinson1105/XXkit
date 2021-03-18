#import <UIKit/UIKit.h>
#import "./XXbuttonLimitShell.h"
#import "./XXtextFieldShell.h"

NS_ASSUME_NONNULL_BEGIN

typedef enum : NSUInteger {
    XXverificationCodeShellModeEmail,
    XXverificationCodeShellModePhoneNumber,
} XXverificationCodeShellMode;

@interface XXverificationCodeShell : QuickComponentBase
@property (nonatomic,strong,readonly) XXtextFieldShell *accountShell;
@property (nonatomic,strong,readonly) XXtextFieldShell *codeShell;
@property (nonatomic,strong,readonly) XXbuttonLimitShell *requestShell;

@property (nonatomic,copy) void (^onRequest)(XXverificationCodeShell *shell, NSString *account);
@property (nonatomic,copy) void (^onTextChanged)(XXverificationCodeShell *shell);

- (void)shellAccount:(UITextField*)account code:(UITextField*)code request:(UIButton*)request mode:(XXverificationCodeShellMode)mode;
@end

NS_ASSUME_NONNULL_END
