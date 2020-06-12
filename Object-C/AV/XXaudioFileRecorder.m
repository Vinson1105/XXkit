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
#pragma mark - Init
+ (XXaudioFileRecorder*)sharedInstance{
    if(nil == _instance){
        static dispatch_once_t onceToken;
        dispatch_once(&onceToken, ^{
            _instance = [XXaudioFileRecorder new];
        });
    }
    return _instance;
}

#pragma mark - Public
- (void)config:(XXaudioFormat*)format{
    self.audioFormat = format;
}
- (BOOL)start:(NSURL*)url{
    if(_isRunning || nil == self.audioFormat || nil == url){
        return NO;
    }
    _isRunning = YES;
    
    NSDictionary *settings = [self settingsFromFormat:self.audioFormat];
    NSError *error;
    _recorder = [[AVAudioRecorder alloc] initWithURL:url settings:settings error:&error];
    if(error){
        NSLog(@"[XXaudioFileReocrder] 创建AVAudioRecorder实例失败，error：%@.",error);
        _isRunning = NO;
        return NO;
    }
    //_recorder.meteringEnabled = YES;
    _recorder.delegate = self;
    
    AVAudioSession *recordSession = [AVAudioSession sharedInstance];
    [recordSession setCategory:AVAudioSessionCategoryPlayAndRecord error:nil];
    [recordSession setActive:YES error:nil];
    
    BOOL ret = [_recorder record];
    if(!ret){
        _isRunning = NO;
    }
    return ret;
}
- (void)pause{
    if(!_isRunning){
        return;
    }
    [_recorder pause];
}
- (BOOL)resume{
    if(!_isRunning){
        return NO;
    }
    return [_recorder record];

}
- (nullable NSURL *)stop{
    if(!_isRunning){
        return nil;
    }
    _isRunning = NO;
    [_recorder stop];
    return _recorder.url;
}

#pragma mark - Private
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

#pragma mark - AVAudioRecorderDelegate
- (void)audioRecorderDidFinishRecording:(AVAudioRecorder *)recorder successfully:(BOOL)flag{
    NSLog(@"[XXaudioFileRecorder] [audioRecorderDidFinishRecording] successfully:%d",flag);
    
}
- (void)audioRecorderEncodeErrorDidOccur:(AVAudioRecorder *)recorder error:(NSError *)error{
    NSLog(@"[XXaudioFileRecorder] [audioRecorderEncodeErrorDidOccur] error:%@",error);

//    if(_events){
//        [_events audioFileRecorder:self didOccurError:error];
//    }
}
@end
