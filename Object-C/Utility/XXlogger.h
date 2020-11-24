//
//  XXlogger.h
//  XXkitProject
//
//  Created by VINSON on 2020/11/12.
//  Copyright © 2020 郭文轩. All rights reserved.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@interface XXfifoBase : NSObject
@property (nonatomic,assign) BOOL enable;
-(instancetype)initWithParam:(NSDictionary*)param;
-(void)push:(NSData*)data;
@end

@interface XXlogger : NSObject
+(void)message:(NSString*)format,...;
+(void)configFifoClass:(Class)cls param:(NSDictionary*)param forName:(NSString*)name;
+(void)setFifoEnable:(BOOL)enable forName:(NSString*)name;
@end

NS_ASSUME_NONNULL_END
