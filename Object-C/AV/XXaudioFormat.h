//
//  XXaudioFormat.h
//  UITestForiOS
//
//  Created by VINSON on 2020/3/24.
//  Copyright © 2020 xxx.UITestForiOS. All rights reserved.
//
/**
 2020.03.24
 通用音频格式参数
 */

#import <Foundation/Foundation.h>
#import <AVFoundation/AVFoundation.h>

NS_ASSUME_NONNULL_BEGIN

@interface XXaudioFormat : NSObject<NSCopying>
/** 采样率，8000、16000、48000等 */
@property (nonatomic,assign) int sampleRate;
/** 通道数  */
@property (nonatomic,assign) int channels;
/** 位深，每一个采样样本所占用的位数，一般是16、24、32 */
@property (nonatomic,assign) int sampleBitSize;
/** 音频编码格式 Audio Data Format Identifiers，一般是kAudioFormatLinearPCM，不同的应用场景又不同的可用值 */
@property (nonatomic,assign) AudioFormatID formatID;
/** AudioStreamBasicDescription Flags，一般是kAudioFormatFlagIsSignedInteger | kAudioFormatFlagIsPacked */
@property (nonatomic,assign) AudioFormatFlags formatFlags;
@end

NS_ASSUME_NONNULL_END
