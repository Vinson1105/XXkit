#import <UIKit/UIKit.h>
#import "XXlimitButtonShell.h"
#import "../UITextField/XXtextFieldShell.h"

NS_ASSUME_NONNULL_BEGIN

@interface XXverificationCodeShell : NSObject
@property (nonatomic,strong,readonly) XXtextFieldShell *accountShell;
@property (nonatomic,strong,readonly) XXtextFieldShell *codeShell;
@property (nonatomic,strong,readonly) XXlimitButtonShell *requestShell;

@property (nonatomic,copy) void (^onRequest)(XXverificationCodeShell *shell, NSString *account);

- (void)shellAccount:(UITextField*)account code:(UITextField*)code request:(UIButton*)request;
@end

NS_ASSUME_NONNULL_END
