//
//  DailyLogFileFifo.m
//  XXkitProject
//
//  Created by 郭文轩 on 2020/11/24.
//  Copyright © 2020 郭文轩. All rights reserved.
//

#import "DailyLogFileFifo.h"
#import "../XXocUtils.h"

static NSString * const kSavePath = @"SavePath";

@interface DailyLogFileFifo()
@property (nonatomic,assign) BOOL available;
@property (nonatomic,copy) NSString *rootDir;
@property (nonatomic,strong) NSDate *dateOfCurrentFile;
@property (nonatomic,strong) NSFileHandle *currentFileHandle;
@property (nonatomic,strong) NSOperationQueue *operationQueue;
@end

@implementation DailyLogFileFifo
- (instancetype)initWithParam:(NSDictionary*)param{
    self = [super init];
    if(self){
        if(param[kSavePath]){
            NSString *path = [[XXocUtils documentAbsolutePathString] stringByAppendingPathComponent:param[kSavePath]];
            if([self createSaveDir:path]){
                self.rootDir = path;
            }
        }
    }
    return self;
}
- (void)dealloc{
    if(_currentFileHandle){
        [_currentFileHandle closeFile];
    }
}
- (void)push:(NSData *)data{
    XXOC_WS;
    [self.operationQueue addOperationWithBlock:^{
        XXOC_SS;
        if (@available(iOS 13.0, *)) {
            NSError *error = nil;
            if(![ss.currentFileHandle writeData:data error:&error]){
                NSLog(@"[%@] 写入失败。（error:%@）", NSStringFromClass(self.class), error);
            }
        } else {
            [ss.currentFileHandle writeData:data];
        }
    }];
}

- (BOOL)createSaveDir:(NSString*)path{
    // 检测目录
    NSFileManager *fileManager = [NSFileManager defaultManager];
    BOOL isDir = NO;
    if([fileManager fileExistsAtPath:path isDirectory:&isDir]){
        if(!isDir){
            return NO;
        }
    }
    else{
        // 创建文件夹
        NSError *error = nil;
        [fileManager createDirectoryAtPath:path withIntermediateDirectories:YES attributes:nil error:&error];
        if(error){
            return NO;
        }
    }
    return YES;
}

- (NSOperationQueue *)operationQueue{
    if(nil == _operationQueue){
        _operationQueue = [[NSOperationQueue alloc] init];
        _operationQueue.maxConcurrentOperationCount = 1;
    }
    return _operationQueue;
}
- (NSFileHandle *)currentFileHandle{
    if(nil == _currentFileHandle){
        // 打开当天文件
        self.dateOfCurrentFile = [NSDate date];
          
        NSInteger flags             = NSCalendarUnitYear | NSCalendarUnitMonth | NSCalendarUnitDay;
        NSDateComponents *comps     = [[NSCalendar currentCalendar] components:flags fromDate:self.dateOfCurrentFile];
        int year                    = (int)[comps year];
        int month                   = (int)[comps month];
        int day                     = (int)[comps day];
        NSString *fileName = [NSString stringWithFormat:@"%d-%02d-%02d",year,month,day];
        NSString *filePath = [self.rootDir stringByAppendingPathComponent:fileName];
        
        NSFileManager *fileManager = [NSFileManager defaultManager];
        if(![fileManager fileExistsAtPath:filePath]){
            [fileManager createFileAtPath:filePath contents:nil attributes:nil];
        }
        _currentFileHandle = [NSFileHandle fileHandleForWritingAtPath:filePath];
        [_currentFileHandle seekToEndOfFile];
    }
    
    return _currentFileHandle;
}
@end
