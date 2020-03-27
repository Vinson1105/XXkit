//
//  XXtools.m
//  iCamSee
//
//  Created by VINSON on 2020/3/27.
//  Copyright © 2020 Macrovideo. All rights reserved.
//

#import "XXtools.h"

#define kDefaultDateFormat @"yyyy-MM-dd HH:mm:ss.SSS"

static XXtools *_instance;

@interface XXtools()
@property (nonatomic,copy) NSDateFormatter *dateFormatter;
@end

@implementation XXtools
+ (XXtools*)sharedInstance{
    if(nil == _instance){
        static dispatch_once_t onceToken;
        dispatch_once(&onceToken, ^{
            _instance = [XXtools new];
        });
    }
    return _instance;
}
- (instancetype)init{
    self = [super init];
    if (self) {
        _dateFormatter = [NSDateFormatter new];
        _dateFormatter.dateFormat = kDefaultDateFormat;
    }
    return self;
}

#pragma mark - JSON
+ (NSString*)jsonStringWithJson:(id)json pretty:(BOOL)pretty{
    if(nil == json) return nil;
    NSError *error;
    NSJSONWritingOptions option;
    if (@available(iOS 11.0, *)) {
        option = pretty?NSJSONWritingPrettyPrinted:NSJSONWritingSortedKeys;
    } else {
        option = pretty?NSJSONWritingPrettyPrinted:kNilOptions;
    }
    NSData *jsonData = [NSJSONSerialization dataWithJSONObject:json options:option error:&error];
    if (!jsonData) {
        return nil;
    }
    else{
        return [[NSString alloc] initWithData:jsonData encoding:NSUTF8StringEncoding];
    }
}
+ (nullable id)jsonWithJsonString:(NSString*)jsonString{
    if (jsonString.length == 0) return nil;

    NSData *jsonData = [jsonString dataUsingEncoding:NSUTF8StringEncoding];
    NSError *error;
    id obj = [NSJSONSerialization JSONObjectWithData:jsonData
                                                        options:NSJSONReadingMutableContainers
                                                          error:&error];
    return nil != error ? nil : obj;
}

#pragma mark - Date
+ (void)setDefaultDateFormatter:(NSDateFormatter*)formatter{
    [XXtools sharedInstance].dateFormatter = formatter;
}
+ (NSString*)currentDateString{
    return [[XXtools sharedInstance].dateFormatter stringFromDate:[NSDate date]];
}
+ (NSString*)currentDateStringWithDateFormat:(NSString*)format timeZone:(NSTimeZone*)timeZone{
    NSDateFormatter *formatter = [NSDateFormatter new];
    formatter.dateFormat = format;
    formatter.timeZone = timeZone;
    return [formatter stringFromDate:[NSDate date]];
}
+ (NSString*)dateStringWithTimestamp:(NSTimeInterval)timestamp{
    return [[XXtools sharedInstance].dateFormatter stringFromDate:[NSDate dateWithTimeIntervalSince1970:timestamp]];
}
+ (NSString*)dateStringWithTimestamp:(NSTimeInterval)timestamp dateFormat:(NSString*)format timeZone:(NSTimeZone*)timeZone{
    NSDateFormatter *formatter = [NSDateFormatter new];
    formatter.dateFormat = format;
    formatter.timeZone = timeZone;
    return [formatter stringFromDate:[NSDate dateWithTimeIntervalSince1970:timestamp]];
}

#pragma mark - File
+ (NSString*)documentAbsolutePath{
    return [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES) objectAtIndex:0];
}
+ (NSString*)fileAbsolutePathInDocument:(NSArray*)nodes{
    NSString *filePath  = [self documentAbsolutePath];
    
    NSEnumerator *enumer    = nodes.objectEnumerator;
    NSString *node          = nil;
    while (nil != (node = enumer.nextObject)) {
        filePath = [filePath stringByAppendingPathComponent:node];
    }
    return filePath;
}
@end
