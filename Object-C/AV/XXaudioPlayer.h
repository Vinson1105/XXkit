//
//  XXaudioPlayer.h
//  NVSDK
//
//  Created by VINSON on 2019/7/3.
//  Copyright © 2019 macrovideo. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <AudioToolbox/AudioToolbox.h>
#import <UIKit/UIKit.h>
#import "XXaudioFormat.h"

NS_ASSUME_NONNULL_BEGIN

@interface XXaudioPlayer : NSObject
@property (nonatomic,assign,readonly) BOOL isRunning;
@property (nonatomic,assign) BOOL mute;
+ (XXaudioPlayer*) sharedInstance;
- (void)config:(XXaudioFormat*)format;
- (void)start;
- (void)stop;
- (void)resetData;
- (BOOL)pushData:(NSData*)data;
@end

NS_ASSUME_NONNULL_END
