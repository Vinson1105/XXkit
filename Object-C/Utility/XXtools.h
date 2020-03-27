//
//  XXtools.h
//  iCamSee
//
//  Created by VINSON on 2020/3/27.
//  Copyright © 2020 Macrovideo. All rights reserved.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@interface XXtools : NSObject
/** JSON */
+ (nullable NSString*)jsonStringWithJson:(id)json pretty:(BOOL)pretty;
+ (nullable id)jsonWithJsonString:(NSString*)jsonString;

/** Date */
+ (void)setDefaultDateFormatter:(NSDateFormatter*)formatter;
+ (NSString*)currentDateString;
+ (NSString*)currentDateStringWithDateFormat:(NSString*)format timeZone:(NSTimeZone*)timeZone;
+ (NSString*)dateStringWithTimestamp:(NSTimeInterval)timestamp;
+ (NSString*)dateStringWithTimestamp:(NSTimeInterval)timestamp dateFormat:(NSString*)format timeZone:(NSTimeZone*)timeZone;

/** File */
+ (NSString*)documentAbsolutePath;
+ (NSString*)fileAbsolutePathInDocument:(NSArray*)nodes;
@end

NS_ASSUME_NONNULL_END
