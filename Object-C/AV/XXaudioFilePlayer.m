//
//  XXaudioFilePlayer.m
//  XXkitProject
//
//  Created by VINSON on 2020/5/28.
//  Copyright © 2020 郭文轩. All rights reserved.
//

#import "XXaudioFilePlayer.h"
#import "../Utility/XXtimer.h"

@interface XXaudioFilePlayer()<AVAudioPlayerDelegate>
@end

@implementation XXaudioFilePlayer
- (instancetype)initWithContentsOfURL:(NSURL *)url fileTypeHint:(NSString *)utiString error:(NSError *__autoreleasing  _Nullable *)outError{
    self = [super initWithContentsOfURL:url fileTypeHint:utiString error:outError];
    self.delegate = self;
    return self;
}
- (instancetype)initWithContentsOfURL:(NSURL *)url error:(NSError *__autoreleasing  _Nullable *)outError{
    self = [super initWithContentsOfURL:url error:outError];
    self.delegate = self;
    return self;
}
- (instancetype)initWithData:(NSData *)data error:(NSError *__autoreleasing  _Nullable *)outError{
    self = [super initWithData:data error:outError];
    self.delegate = self;
    return self;
}
- (instancetype)initWithData:(NSData *)data fileTypeHint:(NSString *)utiString error:(NSError *__autoreleasing  _Nullable *)outError{
    self = [super initWithData:data fileTypeHint:utiString error:outError];
    self.delegate = self;
    return self;
}
- (void)dealloc{
    
}

- (void)initSelf{
    self.delegate = self;
    [self addObserver:self forKeyPath:@"currentTime" options:NSKeyValueObservingOptionNew context:nil];
}
- (void)fainitSelf{
    self.delegate = nil;
    [self removeObserver:self forKeyPath:@"currentTime"];
}

- (void)observeValueForKeyPath:(NSString *)keyPath ofObject:(id)object change:(NSDictionary *)change context:(void *)context{
    if ([keyPath isEqualToString:@"currentTime"]) {
        NSLog(@"[XXaudioFilePlayer] %@",change);
    } else {
        [super observeValueForKeyPath:keyPath ofObject:object change:change context:context];
    }
}
@end
