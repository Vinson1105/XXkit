//
//  XXaudioFilePlayer.h
//  XXkitProject
//
//  Created by VINSON on 2020/5/28.
//  Copyright © 2020 郭文轩. All rights reserved.
//

#import <AVFoundation/AVFoundation.h>

NS_ASSUME_NONNULL_BEGIN

@interface XXaudioFilePlayer : AVAudioPlayer
@property (nonatomic,copy,nullable) void(^onProgressChanged)(XXaudioFilePlayer *player);
@property (nonatomic,copy,nullable) void(^onFinished)(XXaudioFilePlayer *player, BOOL succeed);
@property (nonatomic,copy,nullable) void(^onDecodeError)(XXaudioFilePlayer *player, NSError *error);
@end

NS_ASSUME_NONNULL_END
