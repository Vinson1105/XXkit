//
//  XXaudioRecorder.h
//  UITestForiOS
//
//  Created by VINSON on 2020/3/14.
//  Copyright © 2020 xxx.UITestForiOS. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "XXaudioFormat.h"

NS_ASSUME_NONNULL_BEGIN

@class XXaudioRecorder;
@protocol XXaudioRecorderEvents <NSObject>
-(void)audioRecorder:(XXaudioRecorder*)recorder receiveData:(NSData*)data;
@end

@interface XXaudioRecorder : NSObject
@property (nonatomic,assign,readonly) BOOL isRunning;
@property (nonatomic,weak) id<XXaudioRecorderEvents> events;
+(XXaudioRecorder*) sharedInstance;
-(void)config:(XXaudioFormat*)format;
-(void)start;
-(void)stop;
@end

NS_ASSUME_NONNULL_END

