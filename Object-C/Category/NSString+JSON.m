//
//  NSString+JSON.m
//  XXkitProject
//
//  Created by 郭文轩 on 2020/10/12.
//  Copyright © 2020 郭文轩. All rights reserved.
//

#import "NSString+JSON.h"

@implementation NSString(JSON)
- (id)toJsonObject:(NSError * _Nullable __autoreleasing *)error{
    NSData *data = [self dataUsingEncoding:NSUTF8StringEncoding];
    return [NSJSONSerialization JSONObjectWithData:data options:kNilOptions error:error];
}
-(id)toMutableJsonObject:(NSError * _Nullable *)error{
    NSData *data = [self dataUsingEncoding:NSUTF8StringEncoding];
    return [NSJSONSerialization JSONObjectWithData:data options:kNilOptions|NSJSONReadingMutableContainers error:error];
}

@end
