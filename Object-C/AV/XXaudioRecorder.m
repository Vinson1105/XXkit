//
//  XXaudioRecorder.m
//  UITestForiOS
//
//  Created by VINSON on 2020/3/14.
//  Copyright © 2020 xxx.UITestForiOS. All rights reserved.
//

#import "XXaudioRecorder.h"
#import <AVFoundation/AVFoundation.h>

//#define LIVE_PLAYBACK
#define INPUT_BUS   1
#define OUTPUT_BUS  0

AudioUnit _audioUnit;
AudioBufferList *_buffList;
static XXaudioRecorder *_instance = nil;

@interface XXaudioRecorder()
@property (nonatomic,strong) XXaudioFormat *format;
@property (nonatomic,assign) CGFloat duration;  // 回调间隔
@end

@implementation XXaudioRecorder
+ (XXaudioRecorder *)sharedInstance{
    if(nil == _instance){
        static dispatch_once_t onceToken;
        dispatch_once(&onceToken, ^{
            _instance = [XXaudioRecorder new];
        });
    }
    return _instance;
}
-(void)config:(XXaudioFormat*)format{
    if(_isRunning){
        NSLog(@"[XXaudioRecorder] can not config when recording.");
        return;
    }
    _format = [format copy];
    [self uninit];
    [self initRemoteIO];
}
- (void)start {
    if(_isRunning) return;
    _isRunning = YES;
    AudioOutputUnitStart(_audioUnit);
}
- (void)stop {
    if(!_isRunning) return;
    _isRunning = NO;
    AudioOutputUnitStop(_audioUnit);
}

- (instancetype)init{
    self = [super init];
    if (self) {
        _format = [XXaudioFormat new];
        _format.sampleRate = 8000;
        _format.channels = 1;
        _format.sampleBitSize = 16;
        _format.formatID = kAudioFormatLinearPCM;
        _format.formatFlags = kAudioFormatFlagIsSignedInteger | kAudioFormatFlagIsPacked;
        _duration       = 0.05;
        
        [self initRemoteIO];
    }
    return self;
}
- (void)initRemoteIO {
    AudioUnitInitialize(_audioUnit);
    [self initAudioSession];
    [self initBuffer];
    [self initAudioComponent];
    [self initFormat];
    [self initAudioProperty];
    [self initRecordeCallback];
#ifdef LIVE_PLAYBACK
    [self initPlayCallback];
#endif
}
- (void)dealloc{
    [self uninit];
}

// MARK:组件初始化
- (BOOL)initAudioSession {
    NSError *error;
    // 获取session单例，app唯一
    AVAudioSession *audioSession = [AVAudioSession sharedInstance];
    // 设置session类别
#ifdef LIVE_PLAYBACK
    if(![audioSession setCategory:AVAudioSessionCategoryPlayAndRecord error:&error]){
        NSLog(@"[XXaudioRecorder] error. (LINE:%d) (ERROR:%@)", __LINE__, error);
        return NO;
    }
#else
    if(![audioSession setCategory:AVAudioSessionCategoryPlayAndRecord error:&error]){
        NSLog(@"[XXaudioRecorder] error. (LINE:%d) (ERROR:%@)", __LINE__, error);
        return NO;
    }
#endif
    // 设置采样率
    if(![audioSession setPreferredSampleRate:_format.sampleRate error:&error]){
        NSLog(@"[XXaudioRecorder] error. (LINE:%d) (ERROR:%@)", __LINE__, error);
        return NO;
    }
    // 设置输入通道数（需要在category和mode之后，并session激活之后设置）
//    if(![audioSession setPreferredInputNumberOfChannels:0 error:&error]){
//        NSLog(@"[XXaudioRecorder] error. (LINE:%d) (ERROR:%@ %@)", __LINE__, error, error.description);
//        return NO;
//    }
    // 设置IO缓存时长，即多久回调一次
    if(![audioSession setPreferredIOBufferDuration:_duration error:&error]){
        NSLog(@"[XXaudioRecorder] error. (LINE:%d) (ERROR:%@)", __LINE__, error);
        return NO;
    }
    
    return YES;
}
- (void)initBuffer {
    UInt32 flag = 0;
    AudioUnitSetProperty(_audioUnit,
                         kAudioUnitProperty_ShouldAllocateBuffer,
                         kAudioUnitScope_Output,
                         INPUT_BUS,
                         &flag,
                         sizeof(flag));
    
    // 计算一秒数据长度，通道数 * 采样率 * 样本字节数
    int bufferSize = _format.channels * _format.sampleRate * _format.sampleBitSize / 8;
    
    // 分配bufferlist
    _buffList = (AudioBufferList*)malloc(sizeof(AudioBufferList));
    // 设置list中buffer的数量
    _buffList->mNumberBuffers = 1;
    
    // 设置buffer的通道数
    _buffList->mBuffers[0].mNumberChannels = 1;
    // 设置buffer的大小
    _buffList->mBuffers[0].mDataByteSize = bufferSize;
    // 分配buffer
    _buffList->mBuffers[0].mData = (short *)malloc(bufferSize);
}
- (void)initAudioComponent {
    AudioComponentDescription audioDesc;
    audioDesc.componentType = kAudioUnitType_Output;
    audioDesc.componentSubType = kAudioUnitSubType_RemoteIO;
    audioDesc.componentManufacturer = kAudioUnitManufacturer_Apple;
    audioDesc.componentFlags = 0;
    audioDesc.componentFlagsMask = 0;
    
    AudioComponent inputComponent = AudioComponentFindNext(NULL, &audioDesc);
    AudioComponentInstanceNew(inputComponent, &_audioUnit);
}
- (void)initFormat {
    AudioStreamBasicDescription audioFormat;
    audioFormat.mSampleRate         = _format.sampleRate;
    audioFormat.mFormatID           = kAudioFormatLinearPCM;
    audioFormat.mFormatFlags        = kAudioFormatFlagIsSignedInteger | kAudioFormatFlagIsPacked;
    audioFormat.mFramesPerPacket    = 1;
    audioFormat.mChannelsPerFrame   = _format.channels;
    audioFormat.mBitsPerChannel     = _format.sampleBitSize;
    audioFormat.mBytesPerPacket     = _format.sampleBitSize / 8 * _format.channels;
    audioFormat.mBytesPerFrame      = _format.sampleBitSize / 8 * _format.channels;
    
    AudioUnitSetProperty(_audioUnit,
                         kAudioUnitProperty_StreamFormat,
                         kAudioUnitScope_Output,
                         INPUT_BUS,
                         &audioFormat,
                         sizeof(audioFormat));
#ifdef LIVE_PLAYBACK
    AudioUnitSetProperty(_audioUnit,
                         kAudioUnitProperty_StreamFormat,
                         kAudioUnitScope_Input,
                         OUTPUT_BUS,
                         &audioFormat,
                         sizeof(audioFormat));
#endif
}
- (void)initRecordeCallback {
    AURenderCallbackStruct recordCallback;
    recordCallback.inputProc = RecordCallback;
    recordCallback.inputProcRefCon = (__bridge void *)self;
    AudioUnitSetProperty(_audioUnit,
                         kAudioOutputUnitProperty_SetInputCallback,
                         kAudioUnitScope_Global,
                         INPUT_BUS,
                         &recordCallback,
                         sizeof(recordCallback));
}
- (void)initPlayCallback {
    AURenderCallbackStruct playCallback;
    playCallback.inputProc = PlayCallback;
    playCallback.inputProcRefCon = (__bridge void *)self;
    AudioUnitSetProperty(_audioUnit,
                         kAudioUnitProperty_SetRenderCallback,
                         kAudioUnitScope_Global,
                         OUTPUT_BUS,
                         &playCallback,
                         sizeof(playCallback));
}
- (void)initAudioProperty {
    UInt32 flag = 1;
    
    AudioUnitSetProperty(_audioUnit,
                         kAudioOutputUnitProperty_EnableIO,
                         kAudioUnitScope_Input,
                         INPUT_BUS,
                         &flag,
                         sizeof(flag));
    
#ifdef LIVE_PLAYBACK
    AudioUnitSetProperty(_audioUnit,
                         kAudioOutputUnitProperty_EnableIO,
                         kAudioUnitScope_Input,
                         OUTPUT_BUS,
                         &flag,
                         sizeof(flag));
#endif
}

- (void)uninit{
    if(nil != _buffList){
        free(_buffList->mBuffers[0].mData);
        free(_buffList);
        _buffList = nil;
    }    
    //AudioComponentInstanceDispose(_audioUnit);
    AudioUnitUninitialize(_audioUnit);
}

#pragma mark - 回调
static OSStatus RecordCallback(void *inRefCon,
                               AudioUnitRenderActionFlags *ioActionFlags,
                               const AudioTimeStamp *inTimeStamp,
                               UInt32 inBusNumber,
                               UInt32 inNumberFrames,
                               AudioBufferList *ioData){
    AudioUnitRender(_audioUnit, ioActionFlags, inTimeStamp, inBusNumber, inNumberFrames, _buffList);
    
    XXaudioRecorder *recorder = (__bridge XXaudioRecorder*)(inRefCon);
    if(nil != recorder.events){
        NSData *data = [NSData dataWithBytes:_buffList->mBuffers[0].mData length:_buffList->mBuffers[0].mDataByteSize];
        [recorder.events audioRecorder:recorder receiveData:data];
    }
    return noErr;
}

static OSStatus PlayCallback(void *inRefCon,
                            AudioUnitRenderActionFlags *ioActionFlags,
                            const AudioTimeStamp *inTimeStamp,
                            UInt32 inBusNumber,
                            UInt32 inNumberFrames,
                            AudioBufferList *ioData) {
    AudioUnitRender(_audioUnit, ioActionFlags, inTimeStamp, 1, inNumberFrames, ioData);
    return noErr;
}
@end
