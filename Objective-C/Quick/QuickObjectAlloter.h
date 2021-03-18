//
//  QuickObjectAlloter.h
//  XXkitProject
//
//  Created by VINSON on 2020/11/4.
//  Copyright © 2020 郭文轩. All rights reserved.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@protocol QuickObjectAlloterDelegate <NSObject>
+(nullable id)instanceObject:(id)info;
@end

@interface QuickObjectAlloter : NSObject
+(nullable id)instanceObject:(id)info;
+(void)addAlloterWithClass:(NSString*)alloter;
@end

NS_ASSUME_NONNULL_END
