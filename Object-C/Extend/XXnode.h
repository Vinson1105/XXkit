#import <Foundation/Foundation.h>


@interface XXnode : NSObject

typedef NSString*(^StringBlock)(void);
typedef NSUInteger(^NSUintBlock)(void);
typedef NSInteger(^NSIntBlock)(void);
typedef XXnode*(^ArrayNodeBlock)(uint index);
typedef XXnode*(^DictNodeBlock)(id key);

@property (nonatomic,weak,nullable) id value;
@property (nonatomic,copy,readonly) StringBlock string;
@property (nonatomic,copy,readonly) NSUintBlock nsuint;
@property (nonatomic,copy,readonly) NSIntBlock nsint;
@property (nonatomic,copy,readonly) DictNodeBlock dict;
@property (nonatomic,copy,readonly) ArrayNodeBlock array;

- (instancetype) initWithData:(id)root;
@end
