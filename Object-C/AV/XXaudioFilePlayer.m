//
//  XXaudioFilePlayer.m
//  XXkitProject
//
//  Created by VINSON on 2020/5/28.
//  Copyright © 2020 郭文轩. All rights reserved.
//

#import "XXaudioFilePlayer.h"
#import "../Utility/XXtimer.h"
#import "../XXocUtils.h"

@interface XXaudioFilePlayer()<AVAudioPlayerDelegate>
@property (nonatomic,strong) XXtimer *timer;
@end

@implementation XXaudioFilePlayer
- (instancetype)initWithContentsOfURL:(NSURL *)url fileTypeHint:(NSString *)utiString error:(NSError *__autoreleasing  _Nullable *)outError{
    self = [super initWithContentsOfURL:url fileTypeHint:utiString error:outError];
    [self setup];
    return self;
}
- (instancetype)initWithContentsOfURL:(NSURL *)url error:(NSError *__autoreleasing  _Nullable *)outError{
    self = [super initWithContentsOfURL:url error:outError];
    [self setup];
    return self;
}
- (instancetype)initWithData:(NSData *)data error:(NSError *__autoreleasing  _Nullable *)outError{
    self = [super initWithData:data error:outError];
    [self setup];
    return self;
}
- (instancetype)initWithData:(NSData *)data fileTypeHint:(NSString *)utiString error:(NSError *__autoreleasing  _Nullable *)outError{
    self = [super initWithData:data fileTypeHint:utiString error:outError];
    [self setup];
    return self;
}
- (void)setup{
    self.delegate = self;
    self.timer = [XXtimer timerWithDelay:0 interval:1 singleShot:NO];
    XXOC_WS;
    self.timer.onTimeout = ^(XXtimer * _Nonnull timer, int index) {
        XXOC_SS;
        dispatch_async(dispatch_get_main_queue(), ^{
            if(ss.onProgressUpdate && ss.isPlaying){
                ss.onProgressUpdate(ss);
            }
        });
    };
}
- (void)dealloc{
    [self.timer stop];
}

- (BOOL)play{
    BOOL ret = [super play];
    if(ret){
        [_timer start];
    }
    return ret;
}
- (BOOL)playAtTime:(NSTimeInterval)time{
    BOOL ret = [super playAtTime:time];
    if(ret){
        [_timer start];
    }
    return ret;
}
- (void)pause{
    [super pause];
    [_timer stop];
}

- (void)audioPlayerDidFinishPlaying:(AVAudioPlayer *)player successfully:(BOOL)flag{
    if(_onFinished){
        _onFinished(self,flag);
    }
}
- (void)audioPlayerDecodeErrorDidOccur:(AVAudioPlayer *)player error:(NSError *)error{
    if(_onDecodeError){
        _onDecodeError(self,error);
    }
}
@end
