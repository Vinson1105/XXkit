#import "UIFont+XXstyle.h"
#import "../Category/NSString+JSON.h"

#define kSize   @"Size"
#define kFamily @"Family"

@implementation UIFont (XXstyle)
+(instancetype)styleWithJString:(NSString*)jstring{
    NSDictionary *jdict = [jstring toJsonObject:nil];
    return [UIFont onStyle:jdict];
}
+(instancetype)styleWithJDictionary:(NSDictionary*)jdict{
    return [UIFont onStyle:jdict];
}
+(instancetype)onStyle:(NSDictionary*)jdata{
    if(nil == jdata)
        return [[UIFont alloc] init];
    if(nil != jdata[kSize]){
        if(nil != jdata[kFamily]){
            return [UIFont fontWithName:jdata[kFamily] size:[jdata[kSize] floatValue]];
        }
        else{
            return [UIFont systemFontOfSize:[jdata[kSize] intValue]];
        }
    }
    else{
        return [[UIFont alloc] init];
    }
}
-(void)styleWithJString:(NSString*)jstring{
    NSLog(@"[UIFont+XXstyle] -(void)sytleWithJString:不可用,请使用+(instancetype)sytleWithJString:");
}
-(void)styleWithJDictionary:(NSDictionary*)jdict{
    NSLog(@"[UIFont+XXstyle] -(void)styleWithJDictionary:不可用,请使用+(instancetype)styleWithJDictionary:");
}
@end
