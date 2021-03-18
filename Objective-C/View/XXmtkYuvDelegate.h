//
//  XXmtkYuvDelegate.h
//  XXapp
//
//  Created by VINSON on 2019/5/17.
//  Copyright © 2019 VINSON. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <MetalKit/MetalKit.h>

NS_ASSUME_NONNULL_BEGIN

@interface XXmtkYuvDelegate : NSObject
- (instancetype) initWithMTKView:(MTKView*)view;
- (void) setPixelBufferRef:(CVPixelBufferRef)bufferRef;
- (void) clear;
@end

NS_ASSUME_NONNULL_END
