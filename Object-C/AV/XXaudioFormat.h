//
//  XXaudioFormat.h
//  UITestForiOS
//
//  Created by VINSON on 2020/3/24.
//  Copyright © 2020 xxx.UITestForiOS. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <AVFoundation/AVFoundation.h>

NS_ASSUME_NONNULL_BEGIN

@interface XXaudioFormat : NSObject<NSCopying>
@property (nonatomic,assign) int sampleRate;
@property (nonatomic,assign) int channels;
@property (nonatomic,assign) int sampleBitSize;
@property (nonatomic,assign) AudioFormatID formatID;        // Audio Data Format Identifiers，一般是kAudioFormatLinearPCM
@property (nonatomic,assign) AudioFormatFlags formatFlags;  // AudioStreamBasicDescription Flags，一般是kAudioFormatFlagIsSignedInteger | kAudioFormatFlagIsPacked
@end

NS_ASSUME_NONNULL_END
