//
//  XXaudioPlayer.h
//  NVSDK
//
//  Created by VINSON on 2019/7/3.
//  Copyright © 2019 macrovideo. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <AudioToolbox/AudioToolbox.h>

NS_ASSUME_NONNULL_BEGIN

@interface XXaudioPlayer : NSObject
- (void) start;
- (void) stop;
- (void) reset;
- (void) data:(NSData*)data;
@end

NS_ASSUME_NONNULL_END
