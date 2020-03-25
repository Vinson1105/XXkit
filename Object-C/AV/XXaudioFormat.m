//
//  XXaudioFormat.m
//  UITestForiOS
//
//  Created by VINSON on 2020/3/24.
//  Copyright © 2020 xxx.UITestForiOS. All rights reserved.
//

#import "XXaudioFormat.h"

@implementation XXaudioFormat
- (nonnull id)copyWithZone:(nullable NSZone *)zone {
    XXaudioFormat *obj = [[[self class] allocWithZone:zone] init];
    obj.sampleRate = self.sampleRate;
    obj.channels = self.channels;
    obj.sampleBitSize = self.sampleBitSize;
    obj.formatID = self.formatID;
    obj.formatFlags = self.formatFlags;
    return obj;
}

@end
