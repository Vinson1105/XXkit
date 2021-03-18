#import "UILabel+XXstyle.h"
#import "UIFont+XXstyle.h"
#import "../Utility/NSString+JSON.h"
#import "../Utility/UIColor+Hex.h"
#import "UIView+XXstyle.h"

#define kFont  @"Font"
#define kColor @"Color"

@implementation UILabel (XXstyle)
-(void)styleWithJString:(NSString*)jstring{
    NSDictionary *jdict = [jstring jsonObject];
    [self styleWithJDictionary:jdict];
}
-(void)styleWithJDictionary:(NSDictionary*)jdict{
    if(nil == jdict) return;
    if(nil != jdict[kFont]) self.font = [UIFont styleWithJDictionary:jdict[kFont]];
    if(nil != jdict[kColor]) self.textColor = [UIColor colorWithHexString:jdict[kColor]];
    [super styleWithJDictionary:jdict];
}
@end
