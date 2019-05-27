#import <Foundation/Foundation.h>

@interface NSString(XXextend)
+ (NSString*) currentDateTimeStringWithFotmat:(NSString*)format;
+ (NSString*) currentDateTimeString;
+ (NSString*) dateTimeStringWithFormat:(NSString*)format Timestamp:(NSTimeInterval)timestamp;

+ (NSString*) documentPath;
+ (NSString*) documentPathAtFileName:(NSString*)fileName;
+ (NSString*) documentPathAtFilePath:(NSArray*)filePathNode;
@end
