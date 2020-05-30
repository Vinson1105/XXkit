//
//  XXcoreData.m
//  XXkitProject
//
//  Created by VINSON on 2020/5/13.
//  Copyright © 2020 郭文轩. All rights reserved.
//

#import "XXcoreData.h"
#import "../XXocUtils.h"

static XXcoreData *_instance = nil;

@interface XXcoreData()
@property (nonatomic,strong) NSPersistentContainer *contrainer;
@property (nonatomic,strong) NSManagedObjectModel *model;
@property (nonatomic,strong) NSPersistentStoreCoordinator *coordinator;
@end

@implementation XXcoreData
+ (XXcoreData*)sharedInstance{
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        _instance = [XXcoreData new];
    });
    return _instance;
}

//- (NSManagedObjectContext *)context{
//    if(_contrainer){
//        return _contrainer.viewContext;
//    }
//    else{
//        return _context;
//    }
//}

- (void)configModel:(NSString*)name{
    _contrainer = [[NSPersistentContainer alloc] initWithName:name];
    
    [_contrainer loadPersistentStoresWithCompletionHandler:^(NSPersistentStoreDescription * storeDescription, NSError * error) {
        if(error){
            NSMutableDictionary *dict = [NSMutableDictionary dictionary];
            dict[NSLocalizedDescriptionKey] = @"Failed to initialize the application's saved data";
            dict[NSLocalizedFailureReasonErrorKey] =  @"There was an error creating or loading the application's saved data.";
            dict[NSUnderlyingErrorKey] = error;
            error = [NSError errorWithDomain:@"your error domain" code:666 userInfo:dict];
            abort();
        }
        else{
            NSLog(@"[XXcoreData] %@ %@", storeDescription.URL, storeDescription.type);
        }
    }];
    _context = _contrainer.viewContext;
}

- (void)configModel:(NSString*)name bundle:(nullable NSBundle*)bundle storeType:(NSString*)storeType storeUrl:(NSURL*)storeUrl{
    /// 模型文件
    if(nil == bundle){
        bundle = [NSBundle mainBundle];
    }
    NSURL *modelURL = [bundle URLForResource:name withExtension:@"momd"];
    _model = [[NSManagedObjectModel alloc] initWithContentsOfURL:modelURL];

    /// 创建持久化数据库
    NSError *error = nil;
    _coordinator = [[NSPersistentStoreCoordinator alloc] initWithManagedObjectModel:_model];

    /// 没有数据库迁移
    NSDictionary *options = @{NSMigratePersistentStoresAutomaticallyOption:@(YES),
                              NSInferMappingModelAutomaticallyOption:@(YES),
    };
    if (![_coordinator addPersistentStoreWithType:storeType configuration:nil URL:storeUrl options:options error:&error]) {
        NSMutableDictionary *dict = [NSMutableDictionary dictionary];
        dict[NSLocalizedDescriptionKey] = @"Failed to initialize the application's saved data";
        dict[NSLocalizedFailureReasonErrorKey] =  @"There was an error creating or loading the application's saved data.";
        dict[NSUnderlyingErrorKey] = error;
        error = [NSError errorWithDomain:@"your error domain" code:666 userInfo:dict];
        abort();
    }

    /// 模型、数据库关联上下文
    _context = [[NSManagedObjectContext alloc] initWithConcurrencyType:NSPrivateQueueConcurrencyType];
    _context.persistentStoreCoordinator = _coordinator;
}

#pragma mark - 增加
- (void)insertObject:(NSString*)entryName initHandler:(void(^)(id obj))initHandler error:(NSError* _Nullable*)error{
    /// 生成对象
    id obj = [NSEntityDescription  insertNewObjectForEntityForName:entryName  inManagedObjectContext:self.context];
    
    /// 对象赋值
    if(initHandler){
        initHandler(obj);
    }

    /// 保存操作
    [self.context save:error];
}

#pragma mark - 删除
- (void)deleteObject:(NSString*)entryName predicate:(nullable NSPredicate*)predicate error:(NSError* _Nullable*)error{
    /// 创建查询请求
    NSFetchRequest *request = [NSFetchRequest fetchRequestWithEntityName:entryName];
    if(predicate){
        request.predicate = predicate;
    }
        
    /// 获取目标对象列表
    NSArray *objs = [self.context executeFetchRequest:request error:error];
    
    /// 移除
    for (id obj in objs) {
        [self.context deleteObject:obj];
    }
    
    /// 保存操作
    [self.context save:error];
}
- (void)deleteObject:(NSString*)entryName key:(NSString*)key equelTo:(id)value error:(NSError* _Nullable*)error{
    NSPredicate *predicate = [NSPredicate predicateWithFormat:@"%@ = %@", key, value];
    [self deleteObject:entryName predicate:predicate error:error];
}
- (void)deleteObject:(NSString*)entryName key:(NSString*)key greaterThanTo:(id)value error:(NSError* _Nullable*)error{
    NSPredicate *predicate = [NSPredicate predicateWithFormat:@"%@ > %@", key, value];
    [self deleteObject:entryName predicate:predicate error:error];
}
- (void)deleteObject:(NSString*)entryName key:(NSString*)key greaterThanOrEqualTo:(id)value error:(NSError* _Nullable*)error{
    NSPredicate *predicate = [NSPredicate predicateWithFormat:@"%@ >= %@", key, value];
    [self deleteObject:entryName predicate:predicate error:error];
}
- (void)deleteObject:(NSString*)entryName key:(NSString*)key lessThanTo:(id)value error:(NSError* _Nullable*)error{
    NSPredicate *predicate = [NSPredicate predicateWithFormat:@"%@ < %@", key, value];
    [self deleteObject:entryName predicate:predicate error:error];
}
- (void)deleteObject:(NSString*)entryName key:(NSString*)key lessThanOrEqualTo:(id)value error:(NSError* _Nullable*)error{
    NSPredicate *predicate = [NSPredicate predicateWithFormat:@"%@ <= %@", key, value];
    [self deleteObject:entryName predicate:predicate error:error];
}
- (void)deleteObject:(NSManagedObject*)object error:(NSError* _Nullable*)error{
    [self.context deleteObject:object];
    [self.context save:error];
}

#pragma mark - 查询
- (NSArray*)getObject:(NSString*)entryName predicate:(nullable NSPredicate*)predicate error:(NSError* _Nullable*)error{
    /// 创建查询请求
    NSFetchRequest *request = [NSFetchRequest fetchRequestWithEntityName:entryName];
    if(predicate){
        request.predicate = predicate;
    }
        
    /// 获取目标对象列表
    return [self.context executeFetchRequest:request error:error];
}
- (NSArray*)getObject:(NSString*)entryName key:(NSString*)key equelTo:(id)value error:(NSError* _Nullable*)error{
    NSPredicate *predicate = [NSPredicate predicateWithFormat:@"%@ = %@", key, value];
    return [self getObject:entryName predicate:predicate error:error];
}
- (NSArray*)getObject:(NSString*)entryName key:(NSString*)key greaterThanTo:(id)value error:(NSError* _Nullable*)error{
    NSPredicate *predicate = [NSPredicate predicateWithFormat:@"%@ > %@", key, value];
    return [self getObject:entryName predicate:predicate error:error];
}
- (NSArray*)getObject:(NSString*)entryName key:(NSString*)key greaterThanOrEqualTo:(id)value error:(NSError* _Nullable*)error{
    NSPredicate *predicate = [NSPredicate predicateWithFormat:@"%@ >= %@", key, value];
    return [self getObject:entryName predicate:predicate error:error];
}
- (NSArray*)getObject:(NSString*)entryName key:(NSString*)key lessThanTo:(id)value error:(NSError* _Nullable*)error{
    NSPredicate *predicate = [NSPredicate predicateWithFormat:@"%@ < %@", key, value];
    return [self getObject:entryName predicate:predicate error:error];
}
- (NSArray*)getObject:(NSString*)entryName key:(NSString*)key lessThanOrEqualTo:(id)value error:(NSError* _Nullable*)error{
    NSPredicate *predicate = [NSPredicate predicateWithFormat:@"%@ <= %@", key, value];
    return [self getObject:entryName predicate:predicate error:error];
}
@end
