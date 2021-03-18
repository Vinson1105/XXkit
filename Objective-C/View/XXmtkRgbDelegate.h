//
//  XXmtkRgbDelegate.h
//  XXapp
//
//  Created by VINSON on 2019/6/14.
//  Copyright © 2019 VINSON. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <MetalKit/MetalKit.h>

NS_ASSUME_NONNULL_BEGIN

@interface XXmtkRgbDelegate : NSObject
- (instancetype) initWithMTKView:(MTKView*)view;
- (void) setPixelBufferRef:(CVPixelBufferRef)bufferRef;
- (void) clear;
@end

NS_ASSUME_NONNULL_END
