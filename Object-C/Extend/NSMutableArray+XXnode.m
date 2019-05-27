#import "NSMutableArray+XXnode.h"
#import <objc/runtime.h>

static const void *NodeKey = @"xxnode";

@implementation NSMutableArray(XXnode)
- (ArrayNodeBlock)array{
    XXnode *root = [[XXnode alloc] initWithData:self];
    objc_setAssociatedObject (self , NodeKey, root, OBJC_ASSOCIATION_RETAIN_NONATOMIC);
    return root.array;
}
@end
