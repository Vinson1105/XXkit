//
//  NSString+XXextend.m
//  XXapp
//
//  Created by VINSON on 2019/4/22.
//  Copyright © 2019 VINSON. All rights reserved.
//

#import "NSString+XXextend.h"
#define DATE_FORMAT_DEFAULT @"yyyy-MM-dd HH:mm:ss.SSS"

@implementation NSString(XXextend)
+ (NSDateFormatter*) defaultDateFormtter{
    static NSDateFormatter *defaultDateFormtter = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        defaultDateFormtter = [[NSDateFormatter alloc] init];
        [defaultDateFormtter setDateFormat:DATE_FORMAT_DEFAULT];
    });
    return defaultDateFormtter;
}
+ (NSString*) currentDateTimeStringWithFotmat:(NSString*)format{
    NSString *string = nil;
    
    NSDateFormatter *formatter = [self defaultDateFormtter];
    [formatter setDateFormat:format];
    
    NSDate *date = [NSDate date];
    string = [formatter stringFromDate:date];
    return string;
}
+ (NSString*) currentDateTimeString{
    NSString *string = nil;
    
    NSDateFormatter *formatter = [self defaultDateFormtter];
    [formatter setDateFormat:DATE_FORMAT_DEFAULT];
    
    NSDate *date = [NSDate date];
    string = [formatter stringFromDate:date];
    return string;
}
+ (NSString*) dateTimeStringWithFormat:(NSString*)format Timestamp:(NSTimeInterval)timestamp{
    NSString *string = nil;
    
    NSDateFormatter *formatter = [self defaultDateFormtter];
    [formatter setDateFormat:DATE_FORMAT_DEFAULT];
    
    NSDate *date = [NSDate dateWithTimeIntervalSince1970:timestamp];
    string = [formatter stringFromDate:date];
    return string;
}

+ (NSString*) documentPath{
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    return [paths objectAtIndex:0];
}
+ (NSString*) documentPathAtFileName:(NSString*)fileName{
    NSArray *paths                  = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString *documentsDirectory    = [paths objectAtIndex:0];
    return [documentsDirectory stringByAppendingPathComponent:fileName];
}
+ (NSString*) documentPathAtFilePath:(NSArray*)filePathNode{
    NSArray *paths      = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString *filePath  = [paths objectAtIndex:0];
    
    NSUInteger count            = filePathNode.count;
    for (NSUInteger index = 0; index < count; index++) {
        filePath = [filePath stringByAppendingPathComponent:filePathNode[index]];
    }
    
    return filePath;
}
@end
