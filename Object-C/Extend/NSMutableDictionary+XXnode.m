#import "NSMutableDictionary+XXnode.h"
#import <objc/runtime.h>

static const void *NodeKey = @"xxnode";

@implementation NSMutableDictionary(XXnode)
- (DictNodeBlock)dict{
    XXnode *root = [[XXnode alloc] initWithData:self];
    objc_setAssociatedObject (self , NodeKey, root, OBJC_ASSOCIATION_RETAIN_NONATOMIC);
    return root.dict;
}
@end
