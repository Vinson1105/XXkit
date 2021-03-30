#import "UIView+XXstyle.h"
#import "CALayer+XXstyle.h"
#import "../Category/NSString+JSON.h"
#import "../XXocUtils.h"

#define kLayer @"Layer"
#define kBackgroundColor @"BackgroundColor"

@implementation UIView(XXstyle)
- (void)styleWithJDictionary:(nonnull NSDictionary *)jdicty {
    if(nil == jdicty)   return;
    if(nil != jdicty[kLayer]) [self.layer styleWithJDictionary:jdicty[kLayer]];
    if(nil != jdicty[kBackgroundColor]) self.backgroundColor = [XXocUtils colorFromHexString:jdicty[kBackgroundColor]];
}
- (void)styleWithJString:(nonnull NSString *)jstring {
    NSDictionary *jdict = [jstring toJsonObject:nil];
    [self styleWithJDictionary:jdict];
}
@end
