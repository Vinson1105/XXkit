//
//  XXaudioPlayer.m
//  NVSDK
//
//  Created by VINSON on 2019/7/3.
//  Copyright © 2019 macrovideo. All rights reserved.
//

#define QUEUE_BUFFER_SIZE 4
#define MIN_SIZE_PER_FRAME 2048

#import "XXaudioPlayer.h"
@interface XXaudioPlayer(){
    AudioQueueBufferRef _audioQueueBufferRefArray[QUEUE_BUFFER_SIZE];
    BOOL _audioQueueBufferRefUsedArray[QUEUE_BUFFER_SIZE];
}
@property (nonatomic,assign) AudioQueueRef audioQueueRef;
@property (nonatomic,assign) AudioStreamBasicDescription audioStreamBasicDesctiption;
@property (nonatomic,strong) NSCondition *queueCondition;
@property (nonatomic,assign) BOOL isInterruptRequested;
@property (nonatomic,assign) BOOL isRunning;
@property (nonatomic,strong) NSOperationQueue *dataQueue;

//    AudioQueueRef audioQueue;                                 //音频播放队列
//    AudioStreamBasicDescription _audioDescription;
//    AudioQueueBufferRef audioQueueBuffers[QUEUE_BUFFER_SIZE]; //音频缓存
//    BOOL audioQueueBufferUsed[QUEUE_BUFFER_SIZE];             //判断音频缓存是否在使用
//    NSLock *sysnLock;
//    NSMutableData *tempData;
//    OSStatus osState;
@end

@implementation XXaudioPlayer
- (void) toDescrption:(AudioStreamBasicDescription*)descrption
           SampleRate:(int)sampleRate
           FormatFlag:(AudioFormatFlags)formatFlag
      ChannelPerFrame:(int)channelPerFrame
      FramesPerPacket:(int)framesPerPacket
       BitsPerChannel:(int)bitsPerChannel
       BytesPerPacket:(int)bytesPerPacket{
    
}
- (instancetype)init
{
    self = [super init];
    if (self) {
        _queueCondition         = [[NSCondition alloc] init];
        _dataQueue = [[NSOperationQueue alloc] init];
        _dataQueue.maxConcurrentOperationCount = 1;
        
        _isInterruptRequested = NO;
        _isRunning = NO;
        
        // 播放PCM使用
        if (_audioStreamBasicDesctiption.mSampleRate <= 0) {
            //设置音频参数
            _audioStreamBasicDesctiption.mSampleRate = 8000.0;//采样率
            _audioStreamBasicDesctiption.mFormatID = kAudioFormatLinearPCM;
            // 下面这个是保存音频数据的方式的说明，如可以根据大端字节序或小端字节序，浮点数或整数以及不同体位去保存数据
            _audioStreamBasicDesctiption.mFormatFlags = kLinearPCMFormatFlagIsSignedInteger | kAudioFormatFlagIsPacked;
            //1单声道 2双声道
            _audioStreamBasicDesctiption.mChannelsPerFrame = 1;
            //每一个packet一侦数据,每个数据包下的桢数，即每个数据包里面有多少桢
            _audioStreamBasicDesctiption.mFramesPerPacket = 1;
            //每个采样点16bit量化 语音每采样点占用位数
            _audioStreamBasicDesctiption.mBitsPerChannel = 16;
            _audioStreamBasicDesctiption.mBytesPerFrame = (_audioStreamBasicDesctiption.mBitsPerChannel / 8) * _audioStreamBasicDesctiption.mChannelsPerFrame;
            //每个数据包的bytes总数，每桢的bytes数*每个数据包的桢数
            _audioStreamBasicDesctiption.mBytesPerPacket = _audioStreamBasicDesctiption.mBytesPerFrame * _audioStreamBasicDesctiption.mFramesPerPacket;
        }
        
        // 使用player的内部线程播放 新建输出
        AudioQueueNewOutput(&_audioStreamBasicDesctiption, AudioPlayerAQInputCallback, (__bridge void * _Nullable)(self), nil, 0, 0, &_audioQueueRef);
        
        // 设置音量
        AudioQueueSetParameter(_audioQueueRef, kAudioQueueParam_Volume, 1.0);
        
        // 初始化需要的缓冲区
        for (int i = 0; i < QUEUE_BUFFER_SIZE; i++) {
            _audioQueueBufferRefUsedArray[i] = NO;
            AudioQueueAllocateBuffer(_audioQueueRef, MIN_SIZE_PER_FRAME, &(_audioQueueBufferRefArray[i]));
        }
    }
    return self;
}



// 回调回来把buffer状态设为未使用
static void AudioPlayerAQInputCallback(void* inUserData,AudioQueueRef audioQueueRef, AudioQueueBufferRef audioQueueBufferRef) {
    
    XXaudioPlayer* player = (__bridge XXaudioPlayer*)inUserData;
    [player resetBufferState:audioQueueRef and:audioQueueBufferRef];
}

- (void)resetBufferState:(AudioQueueRef)audioQueueRef and:(AudioQueueBufferRef)audioQueueBufferRef {
    for (int i = 0; i < QUEUE_BUFFER_SIZE; i++) {
        if (audioQueueBufferRef == _audioQueueBufferRefArray[i]) {
            _audioQueueBufferRefUsedArray[i] = NO;
        }
    }
}

// ************************** 内存回收 **********************************

- (void)dealloc {
    if (_audioQueueRef != nil) {
        AudioQueueStop(_audioQueueRef, true);
    }
    _audioQueueRef = nil;
    
    for (int i = 0; i < QUEUE_BUFFER_SIZE; i++) {
        AudioQueueFreeBuffer(_audioQueueRef, _audioQueueBufferRefArray[i]);
    }
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
- (void) reset{
    [_dataQueue cancelAllOperations];
    AudioQueueReset(_audioQueueRef);
    [_dataQueue addOperationWithBlock:^{
        for (int i = 0; i < QUEUE_BUFFER_SIZE; i++) {
            self->_audioQueueBufferRefUsedArray[i] = NO;
        }
    }];
}
- (void) data:(NSData*)data{
    [_dataQueue addOperationWithBlock:^{
        int i = 0;
        while (self.isRunning && !self.isInterruptRequested) {
            if (!self->_audioQueueBufferRefUsedArray[i]) {
                self->_audioQueueBufferRefUsedArray[i] = YES;
                break;
            }
            else {
                i++;
                if (i >= QUEUE_BUFFER_SIZE) {
                    i = 0;
                }
            }
            [NSThread sleepForTimeInterval:0.005];
        }
        if (!self.isRunning || self.isInterruptRequested) {
            [self.queueCondition unlock];
            return;
        }
        
        AudioQueueBufferRef audioQueueBufferRef = self->_audioQueueBufferRefArray[i];
        audioQueueBufferRef->mAudioDataByteSize = (unsigned int)data.length;
        memcpy(audioQueueBufferRef->mAudioData, data.bytes, data.length);
        AudioQueueEnqueueBuffer(self.audioQueueRef, audioQueueBufferRef, 0, NULL);
    }];
}
@end
