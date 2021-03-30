#import "CALayer+XXstyle.h"
#import "../Category/NSString+JSON.h"
#import "../XXocUtils.h"
 
#define kBorderWidth    @"BoarderWidth"
#define kBorderColor    @"BorderColor"
#define kRadius         @"Radius"


@implementation CALayer(XXstyle)
+(instancetype)styleWithJString:(NSString*)jstring{
    CALayer *layer = [[CALayer alloc] init];
    [layer styleWithJString:jstring];
    return layer;
}
+(instancetype)styleWithJDictionary:(NSDictionary*)jdict{
    CALayer *layer = [[CALayer alloc] init];
    [layer styleWithJDictionary:jdict];
    return layer;
}
-(void)styleWithJString:(NSString*)jstring{
    NSDictionary *jdict = [jstring toJsonObject:nil];
    [self onStyle:jdict];
}
-(void)styleWithJDictionary:(NSDictionary*)jdict{
    [self onStyle:jdict];
}

-(void)onStyle:(NSDictionary*)jdict{
    if(nil == jdict) return;
    if(nil != jdict[kBorderWidth]) self.borderWidth = [jdict[kBorderWidth] floatValue];
    if(nil != jdict[kBorderColor]) self.borderColor = [XXocUtils colorFromHexString:jdict[kBorderColor]].CGColor;
    if(nil != jdict[kRadius]) self.cornerRadius = [jdict[kRadius] floatValue];
}
@end
