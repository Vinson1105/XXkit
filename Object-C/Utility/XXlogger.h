//
//  XXlogger.h
//  XXkitProject
//
//  Created by VINSON on 2020/11/12.
//  Copyright © 2020 郭文轩. All rights reserved.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@interface XXlogger : NSObject
+(void)normal:(NSString*)msg;
+(void)raw:(NSString*)msg;
+(void)enableServerIP:(NSString*)ip serverPort:(NSInteger)port;
+(void)enableLocalPathNodes:(NSArray<NSString*>*)nodes;
@end

NS_ASSUME_NONNULL_END
