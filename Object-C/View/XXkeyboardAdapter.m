#import "XXkeyboardAdapter.h"

static XXkeyboardAdapter *_instance = nil;
@interface XXkeyboardAdapter()
@property (nonatomic,strong) NSMapTable<NSString*,UIView*> *nameToView;
@property (nonatomic,strong) NSMapTable<NSString*,UIView*> *nameToMainView;
@end

@implementation XXkeyboardAdapter
#pragma mark - 公共接口: <注册> <删除>
//+ (BOOL) registerMainView:(UIView*)mainView View:(UIView*)view Name:(NSString*)name;
//+ (void) remove:(NSString*)name;

#pragma mark - 构建析构
+ (XXkeyboardAdapter*) sharedInstance{
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        _instance = [[XXkeyboardAdapter alloc] init];
    });
    return _instance;
}
- (instancetype)init{
    self = [super init];
    if (self) {
        
    }
    return self;
}
@end
