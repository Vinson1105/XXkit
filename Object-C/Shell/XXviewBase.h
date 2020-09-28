//
//  XXviewBase.h
//  XXkitProject
//
//  Created by VINSON on 2020/9/28.
//  Copyright © 2020 郭文轩. All rights reserved.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@protocol XXviewBase
/**
 自定义事件回调，一般需要传递到应用层
 @param source 事件的产生源
 @param event 事件的名称
 @param info 事件携带的信息
 */
@property (nonatomic,copy,nullable) void(^onEventOccured)(id<XXviewBase> source, NSString *event, id info);

/**
 数据提交，一般是用户通过一些操作修改了id<XXviewBase>实例数据，该实例需要将其最新的数据，往上层的数据存放中提交保存
 @param data 当前的实际数据
 */
@property (nonatomic,copy,nullable) void(^onDataPost)(id data);

/**
 重置数据
 */
- (void)resetData:(id)data;

/**
 委派任务
 @param event 任务名称
 @param info 任务的一些信息
 */
- (void)doEvent:(NSString*)event info:(nullable id)info;
@end



NS_ASSUME_NONNULL_END
