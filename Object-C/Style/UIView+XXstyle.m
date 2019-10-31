#import "UIView+XXstyle.h"
#import "CALayer+XXstyle.h"
#import "../Utility/NSString+JSON.h"
#import "../Utility/UIColor+Hex.h"

#define kLayer @"Layer"
#define kBackgroundColor @"BackgroundColor"

@implementation UIView(XXstyle)
- (void)styleWithJDictionary:(nonnull NSDictionary *)jdicty {
    if(nil == jdicty)   return;
    if(nil != jdicty[kLayer]) [self.layer styleWithJDictionary:jdicty[kLayer]];
    if(nil != jdicty[kBackgroundColor]) self.backgroundColor = [UIColor colorWithHexString:jdicty[kBackgroundColor]];
}
- (void)styleWithJString:(nonnull NSString *)jstring {
    NSDictionary *jdict = [jstring jsonObject];
    [self styleWithJDictionary:jdict];
}
@end
