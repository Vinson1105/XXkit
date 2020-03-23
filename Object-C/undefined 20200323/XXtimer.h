//
//  XXtimer.h
//  iCamSee
//
//  Created by VINSON on 2019/11/28.
//  Copyright © 2019 Macrovideo. All rights reserved.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@interface XXtimer : NSObject
@property (nonatomic,copy) void(^onTimeout)(int times);
@property (nonatomic,assign,readonly) BOOL isRunning;
+(XXtimer*)timerWithDelay:(NSTimeInterval)delay interval:(NSTimeInterval)interval singleShot:(BOOL)singleShot;

-(instancetype)initWithDelay:(NSTimeInterval)delay interval:(NSTimeInterval)interval singleShot:(BOOL)singleShot;
-(void)resetDelay:(NSTimeInterval)delay interval:(NSTimeInterval)interval singleShot:(BOOL)singleShot;
-(void)start;
-(void)stop;
@end

NS_ASSUME_NONNULL_END
