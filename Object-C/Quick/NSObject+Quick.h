//
//  NSObject+Quick.h
//  XXkitProject
//
//  Created by 郭文轩 on 2020/10/12.
//  Copyright © 2020 郭文轩. All rights reserved.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@interface NSObject(Quick)
@property (nonatomic,copy) NSString *quick_name;
-(void)quick_reset:(id)data;
-(void)quick_resetFromJFile:(NSString*)file bundle:(nullable NSBundle*)bundle;
@end

NS_ASSUME_NONNULL_END
