//
//  QuickComponentBase.m
//  XXkitProject
//
//  Created by 郭文轩 on 2020/10/14.
//  Copyright © 2020 郭文轩. All rights reserved.
//

#import "QuickComponentBase.h"

@interface QuickComponentBase() <XXquickComponentDelegate>

@end

@implementation QuickComponentBase
+ (BOOL)quick:(id)obj kvdata:(NSDictionary *)kvdata{
    NSEnumerator *keyEnumer = kvdata.keyEnumerator;
    NSString *key = nil;
    while (nil != (key = keyEnumer.nextObject)) {
        [self obj:obj key:key value:kvdata[key]];
    }
    return YES;
}

+ (NSString *)targetClass{
    @throw [NSExpression expressionWithFormat:@"[QuickComponentBase] [targetClass] 需要重载。"];
    return nil;
}
+ (void)obj:(id)obj key:(NSString*)key value:(id)value{
    @throw [NSExpression expressionWithFormat:@"[QuickComponentBase] [obj: key: value:] 需要重载。"];
}
@end
