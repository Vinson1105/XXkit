//
//  XXaudioPlayer.m
//  NVSDK
//
//  Created by VINSON on 2019/7/3.
//  Copyright © 2019 macrovideo. All rights reserved.
//

#define kBufferCountMax     4
#define kBufferSizeMax      2048
#define kBufferAvailabled   0
#define kBufferBusy         1

#import "XXaudioPlayer.h"

static XXaudioPlayer *_instance = nil;
static void AudioPlayerAQInputCallback(void* inUserData, AudioQueueRef audioQueueRef, AudioQueueBufferRef audioQueueBufferRef) {
    // 回调回来把buffer状态设为未使用
    *(int8_t*)(audioQueueBufferRef->mUserData) = kBufferAvailabled;
}

@interface XXaudioPlayer(){
    AudioQueueBufferRef _audioQueueBufferRefs[kBufferCountMax];
}
@property (nonatomic,assign) AudioQueueRef audioQueueRef;               // 音频队列引用
@property (nonatomic,assign) AudioStreamBasicDescription audioStreamBasicDesctiption;   // 音频格式
@property (nonatomic,strong) NSCondition *queueCondition;               // 队列同步锁
@property (nonatomic,strong) NSOperationQueue *dataQueue;

@property (nonatomic,assign) BOOL isInterruptRequested;
@property (nonatomic,strong) XXaudioFormat *format;
@property (nonatomic,assign) CGFloat volume; // 0.0 - 1.0
@end

@implementation XXaudioPlayer
+(XXaudioPlayer*) sharedInstance{
    if (nil == _instance) {
        static dispatch_once_t onceToken;
        dispatch_once(&onceToken, ^{
            _instance = [XXaudioPlayer new];
        });
    }
    return _instance;
}
- (instancetype)init{
    self = [super init];
    if (self) {
        _queueCondition = [[NSCondition alloc] init];
        _dataQueue      = [[NSOperationQueue alloc] init];
        _dataQueue.maxConcurrentOperationCount = 1;
        
        _isInterruptRequested   = NO;
        _isRunning              = NO;
        _mute                   = NO;
        _volume                 = 1.0;
        
        _format = [XXaudioFormat new];
        _format.sampleRate = 8000;
        _format.channels = 1;
        _format.sampleBitSize = 16;
        _format.formatID = kAudioFormatLinearPCM;
        _format.formatFlags = kAudioFormatFlagIsSignedInteger | kAudioFormatFlagIsPacked;
        
        [self initFormat];
    }
    return self;
}
-(void)initFormat{
    //设置音频参数
    _audioStreamBasicDesctiption.mSampleRate = _format.sampleRate;
    _audioStreamBasicDesctiption.mFormatID = kAudioFormatLinearPCM;
    // 音频格式
    _audioStreamBasicDesctiption.mFormatFlags = kLinearPCMFormatFlagIsSignedInteger | kAudioFormatFlagIsPacked;
    // 每一帧数据的通道数量，即音频的通道数，1单声道 2双声道
    _audioStreamBasicDesctiption.mChannelsPerFrame = _format.channels;
    //每一个packet一侦数据,每个数据包下的桢数，即每个数据包里面有多少桢
    _audioStreamBasicDesctiption.mFramesPerPacket = 1;
    //每个采样点16bit量化 语音每采样点占用位数
    _audioStreamBasicDesctiption.mBitsPerChannel = _format.sampleBitSize;
    _audioStreamBasicDesctiption.mBytesPerFrame = (_audioStreamBasicDesctiption.mBitsPerChannel / 8) * _audioStreamBasicDesctiption.mChannelsPerFrame;
    //每个数据包的bytes总数，每桢的bytes数*每个数据包的桢数
    _audioStreamBasicDesctiption.mBytesPerPacket = _audioStreamBasicDesctiption.mBytesPerFrame * _audioStreamBasicDesctiption.mFramesPerPacket;
    
    // 使用player的内部线程播放 新建输出
    AudioQueueNewOutput(&_audioStreamBasicDesctiption, AudioPlayerAQInputCallback, (__bridge void * _Nullable)(self), nil, 0, 0, &_audioQueueRef);
    
    // 设置音量
    AudioQueueSetParameter(_audioQueueRef, kAudioQueueParam_Volume, _volume);
    
    // 初始化需要的缓冲区
    for (int index = 0; index < kBufferCountMax; index++) {
        AudioQueueAllocateBuffer(_audioQueueRef, kBufferSizeMax, &(_audioQueueBufferRefs[index]));
        _audioQueueBufferRefs[index]->mUserData = malloc(1);
        *(int8_t*)(_audioQueueBufferRefs[index]->mUserData) = kBufferAvailabled;
    }
}
- (void)uninit{
    if (_audioQueueRef != nil) {
        AudioQueueStop(_audioQueueRef, true);
        AudioQueueDispose(_audioQueueRef, YES);
    }
    _audioQueueRef = nil;
    
    for (int index = 0; index < kBufferCountMax; index++) {
        free(_audioQueueBufferRefs[index]->mUserData);
        AudioQueueFreeBuffer(_audioQueueRef, _audioQueueBufferRefs[index]);
    }
}
- (void)dealloc {
    [self uninit];
}

- (void)setMute:(BOOL)mute{
    if(_mute == mute) return;
    _mute = mute;
    AudioQueueSetParameter(_audioQueueRef, kAudioQueueParam_Volume, _mute?0:_volume);
}

- (void)config:(XXaudioFormat *)format{
    if(_isRunning){
        NSLog(@"[XXaudioPlayer] can not config when playing.");
        return;
    }
    _format = [format copy];
    [self uninit];
    [self initFormat];
}
- (void) start{
    if (_isRunning) {
        return;
    }
    _isInterruptRequested = NO;
    _isRunning = YES;
    AudioQueueStart(_audioQueueRef, NULL);
}
- (void) stop{
    if (!_isRunning) {
        return;
    }
    _isInterruptRequested = YES;
    _isRunning = NO;
    [_dataQueue cancelAllOperations];
    AudioQueueStop(_audioQueueRef, YES);
    AudioQueueReset(_audioQueueRef);
}
- (void)resetData{
    [_dataQueue cancelAllOperations];
    AudioQueueReset(_audioQueueRef);
    for (int index = 0; index < kBufferCountMax; index++) {
        int8_t *userData = _audioQueueBufferRefs[index]->mUserData;
        *userData = kBufferAvailabled;
    }
}
- (BOOL)pushData:(NSData*)data{
    if(nil == data){
        NSLog(@"[XXaudioPlayer] it is unavailable that data is nil.");
        return NO;
    }
    if(!_isRunning || _isInterruptRequested) {
        NSLog(@"[XXaudioPlayer] can not push data when player was stopped.");
        return NO;
    }
    if(data.length > kBufferSizeMax){
        NSLog(@"[XXaudioPlayer] can not push data when data size(%d) > buffer size(%d).", (int)data.length, kBufferSizeMax);
        return NO;
    }
    
    AudioQueueBufferRef targetBufferRef = NULL;
    for (int index = 0; index < kBufferCountMax; index++) {
        int8_t *userData = _audioQueueBufferRefs[index]->mUserData;
        if(0 == *userData){
            *userData = kBufferBusy;
            targetBufferRef = _audioQueueBufferRefs[index];
            break;
        }
    }
    if(NULL == targetBufferRef){
        NSLog(@"[XXaudioPlayer] not avaliable buffer");
        return NO;
    }
    
    __weak typeof(self) weakSelf = self;
    [_dataQueue addOperationWithBlock:^{
        __strong typeof(weakSelf) strongSelf = weakSelf;
        if (!strongSelf.isRunning || strongSelf.isInterruptRequested) {
            if(NULL != targetBufferRef){
                int8_t *userData = targetBufferRef->mUserData;
                *userData = kBufferAvailabled;
            }
            return;
        }
        
        targetBufferRef->mAudioDataByteSize = (unsigned int)data.length;
        memcpy(targetBufferRef->mAudioData, data.bytes, data.length);
        AudioQueueEnqueueBuffer(strongSelf.audioQueueRef, targetBufferRef, 0, NULL);
    }];
    return YES;
}
@end
