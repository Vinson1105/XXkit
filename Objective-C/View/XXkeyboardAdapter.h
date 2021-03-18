#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN

@interface XXkeyboardAdapter : NSObject
+ (BOOL) registerMainView:(UIView*)mainView UITextField:(UIView*)view Name:(NSString*)name;
+ (void) remove:(NSString*)name;
@end

NS_ASSUME_NONNULL_END
