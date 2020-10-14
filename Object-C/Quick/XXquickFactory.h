//
//  XXquickFactory.h
//  XXkitProject
//
//  Created by 郭文轩 on 2020/10/12.
//  Copyright © 2020 郭文轩. All rights reserved.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@protocol XXquickComponentDelegate <NSObject>
-(NSString*)targetClass;
-(BOOL)quick:(id)obj kvdata:(NSDictionary*)kvdate;
@end

@interface XXquickFactory : NSObject
+(BOOL)quick:(id)obj data:(id)data;
+(void)installComponent:(id<XXquickComponentDelegate>)component;
@end

NS_ASSUME_NONNULL_END
