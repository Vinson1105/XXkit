#import "UILabel+XXstyle.h"
#import "UIFont+XXstyle.h"
#import "../Category/NSString+JSON.h"
#import "UIView+XXstyle.h"
#import "../XXocUtils.h"

#define kFont  @"Font"
#define kColor @"Color"

@implementation UILabel (XXstyle)
-(void)styleWithJString:(NSString*)jstring{
    NSDictionary *jdict = [jstring toJsonObject:nil];
    [self styleWithJDictionary:jdict];
}
-(void)styleWithJDictionary:(NSDictionary*)jdict{
    if(nil == jdict) return;
    if(nil != jdict[kFont]) self.font = [UIFont styleWithJDictionary:jdict[kFont]];
    if(nil != jdict[kColor]) self.textColor = [XXocUtils colorFromHexString:jdict[kColor]];
    [super styleWithJDictionary:jdict];
}
@end
