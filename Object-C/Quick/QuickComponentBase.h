//
//  QuickComponentBase.h
//  XXkitProject
//
//  Created by 郭文轩 on 2020/10/14.
//  Copyright © 2020 郭文轩. All rights reserved.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@protocol XXquickComponentDelegate <NSObject>
+(NSString*)targetClass;
+(BOOL)quick:(id)obj kvdata:(NSDictionary*)kvdate;
@end

@interface QuickComponentBase : NSObject
+(NSString *)targetClass;
+(void)obj:(id)obj key:(NSString*)key value:(id)value;
@end

NS_ASSUME_NONNULL_END
