#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN
@interface XXnode : NSObject
typedef NSString* _Nullable (^StringBlock)(void);
typedef NSUInteger(^NSUintBlock)(void);
typedef NSInteger(^NSIntBlock)(void);
typedef  XXnode* _Nullable (^ArrayNodeBlock)(uint index);
typedef  XXnode* _Nullable (^DictNodeBlock)(id key);

@property (nonatomic,weak,nullable) id value;
@property (nonatomic,copy,readonly) StringBlock string;
@property (nonatomic,copy,readonly) NSUintBlock nsuint;
@property (nonatomic,copy,readonly) NSIntBlock nsint;
@property (nonatomic,copy,readonly) DictNodeBlock dict;
@property (nonatomic,copy,readonly) ArrayNodeBlock array;

- (instancetype) initWithData:(id)root;
@end
NS_ASSUME_NONNULL_END
