//
//  NSFileHandle+XXextend.m
//  XXapp
//
//  Created by VINSON on 2019/4/22.
//  Copyright © 2019 VINSON. All rights reserved.
//

#import "NSFileHandle+XXextend.h"

@implementation NSFileHandle(XXextend)
+ (instancetype)fileHandleForWritingInDocumentAtFileName:(NSString *)fileName{
    NSArray *paths                  = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString *documentsDirectory    = [paths objectAtIndex:0];
    NSString *filePath              = [documentsDirectory stringByAppendingPathComponent:fileName];
    
    NSFileManager *fileManager = [NSFileManager defaultManager];
    if(![fileManager fileExistsAtPath:fileName]){
        [fileManager createFileAtPath:filePath contents:nil attributes:nil];
    }
    
    return [NSFileHandle fileHandleForWritingAtPath:filePath];
}
+ (instancetype)fileHandleForWritingInDocumentAtFilePath:(NSArray *)filePathNode{
    NSArray *paths      = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString *filePath  = [paths objectAtIndex:0];
    
    NSFileManager *fileManager  = [NSFileManager defaultManager];
    NSUInteger count            = filePathNode.count;
    for (NSUInteger index = 0; index < count; index++) {
        filePath = [filePath stringByAppendingPathComponent:filePathNode[index]];
        
        if ([fileManager fileExistsAtPath:filePath]) {
            continue;
        }
        
        if (index == count - 1) {
            [fileManager createFileAtPath:filePath contents:nil attributes:nil];
        }
        else{
            [fileManager createDirectoryAtPath:filePath withIntermediateDirectories:NO attributes:nil error:nil];
        }
    }
    
    return [NSFileHandle fileHandleForWritingAtPath:filePath];
}
@end
