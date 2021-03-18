//
//  XXyuvView.h
//  XXapp
//
//  Created by VINSON on 2019/5/10.
//  Copyright © 2019 VINSON. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <MetalKit/MetalKit.h>
#import <Metal/Metal.h>

NS_ASSUME_NONNULL_BEGIN

@interface XXyuvView : MTKView
- (void) setData:(NSData*)data Width:(int)width Height:(int)height;
- (void) setImageBufferRef:(CVImageBufferRef)bufferRef;
- (void) clear;
@end

NS_ASSUME_NONNULL_END
