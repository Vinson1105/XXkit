//
//  NSString+JSON.m
//  XXkitDemo
//
//  Created by VINSON on 2019/10/18.
//  Copyright © 2019 VINSON. All rights reserved.
//

#import "NSString+JSON.h"

@implementation NSString (JSON)
+(NSString*)jsonStringFromJSONObject:(id)jsonObject{
    if(nil == jsonObject) return nil;
    NSError *error;
    NSData *jsonData = [NSJSONSerialization dataWithJSONObject:jsonObject options:NSJSONWritingPrettyPrinted error:&error];

    if (!jsonData) {
        return nil;
    }
    else{
        return [[NSString alloc] initWithData:jsonData encoding:NSUTF8StringEncoding];
    }
}

-(id)jsonObject{
    if (self.length == 0) return nil;

    NSData *jsonData = [self dataUsingEncoding:NSUTF8StringEncoding];
    NSError *error;
    id obj = [NSJSONSerialization JSONObjectWithData:jsonData
                                                        options:NSJSONReadingMutableContainers
                                                          error:&error];
    return nil != error ? nil : obj;
}
@end
