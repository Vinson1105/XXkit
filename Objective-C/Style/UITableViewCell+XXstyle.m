#import "UITableViewCell+XXstyle.h"
#import "UILabel+XXstyle.h"
#import "../Category/NSString+JSON.h"

#define kTitle @"Title"
#define kDetail @"Detail"

@implementation UITableViewCell(XXstyle)
- (void)styleWithJDictionary:(nonnull NSDictionary *)jdict {
    if(nil == jdict) return;
    if(nil != jdict[kTitle]) [self.textLabel styleWithJDictionary:jdict[kTitle]];
    if(nil != jdict[kDetail]) [self.detailTextLabel styleWithJDictionary:jdict[kDetail]];
}
- (void)styleWithJString:(nonnull NSString *)jstring {
    NSDictionary *jdict = [jstring toJsonObject:nil];
    [self styleWithJDictionary:jdict];
}
@end
