//
//  XXaudioFileRecorder.m
//  XXkitProject
//
//  Created by VINSON on 2020/5/8.
//  Copyright © 2020 郭文轩. All rights reserved.
//

#import "XXaudioFileRecorder.h"
#import <AVFoundation/AVFoundation.h>
#import <CoreAudio/CoreAudioTypes.h>

static XXaudioFileRecorder *_instance = nil;

@interface XXaudioFileRecorder()<AVAudioRecorderDelegate>
@property (nonatomic,strong) AVAudioRecorder *recorder;
@property (nonatomic,copy) XXaudioFormat *audioFormat;
@end

@implementation XXaudioFileRecorder
+ (XXaudioFileRecorder*)sharedInstance{
    if(nil == _instance){
        static dispatch_once_t onceToken;
        dispatch_once(&onceToken, ^{
            _instance = [XXaudioFileRecorder new];
        });
    }
    return _instance;
}

- (void)config:(XXaudioFormat*)format{
    _audioFormat = format;
}

- (BOOL)start:(NSURL *)url{
    
}
- (void)pause{
    
}
- (void)resume{
    
}
- (NSURL *)stop{
    
}

- (NSDictionary*)settingsFromFormat:(XXaudioFormat*)format{
    NSMutableDictionary *settings = [NSMutableDictionary dictionary];
    /// 音频压缩格式
    settings[AVFormatIDKey] = @(format.formatID);
    /// 音频采样率
    settings[AVSampleRateKey] = @(format.sampleRate);
    /// 音频通道数
    settings[AVNumberOfChannelsKey] = @(format.channels);
    /// 音频位深（样品的位数）
    settings[AVLinearPCMBitDepthKey] = @(format.sampleBitSize);
    /// 是否使用浮点数采样
    settings[AVLinearPCMIsFloatKey] = format.formatFlags&kLinearPCMFormatFlagIsFloat ? @(YES) : @(NO);
    /// 是否为大端模式
    /// TODO: 大小端判断
    return settings;
}
@end
