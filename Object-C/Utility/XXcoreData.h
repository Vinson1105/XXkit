/**
 2020.05.13
 CoreData的使用
 1、File->New->DataModel修改文件名（ModelName）并创建
 2、打开***.xcdatamodeld文件，点击‘Add Entity’创建并编辑条目名称（EntityName）
 3、编辑对应的Attributes、Relationships、Fetched Properties，Editor->Create ManagedObjectSubClass
 */

#import <Foundation/Foundation.h>
#import <CoreData/CoreData.h>

NS_ASSUME_NONNULL_BEGIN

@interface XXcoreData : NSObject
/** Core Data的上下文对象 */
@property (nonatomic,strong,readonly) NSManagedObjectContext *context;

/** 单例 */
+ (XXcoreData*)sharedInstance;

- (void)configModel:(NSString*)name;

/**
 配置模型
 @param name 模型的文件名称，即【***】.xcdatamodeld中的***
 @param bundle 模型文件所在的包，为nil时，使用mainBundle
 @param storeType 数据库类型，支持类型查看‘Persistent Store Types’
 @param storeUrl 数据库保存路径
 */
- (void)configModel:(NSString*)name bundle:(nullable NSBundle*)bundle storeType:(NSString*)storeType storeUrl:(NSURL*)storeUrl;

/**
 插入对象
 @param entryName 对象名称
 @param initHandler 新建对象是初始化回调
 */
- (void)insertObject:(NSString*)entryName initHandler:(void(^)(id obj))initHandler error:(NSError* _Nullable*)error;

/**
 删除对象
 @param entryName 对象名称
 @param predicate 目标对象条件，为nil时，删除该名称的所有对象
 */
- (void)deleteObject:(NSString*)entryName predicate:(nullable NSPredicate*)predicate error:(NSError* _Nullable*)error;
- (void)deleteObject:(NSString*)entryName key:(NSString*)key equelTo:(id)value error:(NSError* _Nullable*)error;
- (void)deleteObject:(NSString*)entryName key:(NSString*)key greaterThanTo:(id)value error:(NSError* _Nullable*)error;
- (void)deleteObject:(NSString*)entryName key:(NSString*)key greaterThanOrEqualTo:(id)value error:(NSError* _Nullable*)error;
- (void)deleteObject:(NSString*)entryName key:(NSString*)key lessThanTo:(id)value error:(NSError* _Nullable*)error;
- (void)deleteObject:(NSString*)entryName key:(NSString*)key lessThanOrEqualTo:(id)value error:(NSError* _Nullable*)error;
- (void)deleteObject:(NSManagedObject*)object error:(NSError* _Nullable*)error;

/**
 查找对象
 @param entryName 对象名称
 @param predicate 目标对象条件，为nil时，返回该名称的所有对象
 */
- (NSArray*)getObject:(NSString*)entryName predicate:(nullable NSPredicate*)predicate error:(NSError* _Nullable*)error;
- (NSArray*)getObject:(NSString*)entryName key:(NSString*)key equelTo:(id)value error:(NSError* _Nullable*)error;
- (NSArray*)getObject:(NSString*)entryName key:(NSString*)key greaterThanTo:(id)value error:(NSError* _Nullable*)error;
- (NSArray*)getObject:(NSString*)entryName key:(NSString*)key greaterThanOrEqualTo:(id)value error:(NSError* _Nullable*)error;
- (NSArray*)getObject:(NSString*)entryName key:(NSString*)key lessThanTo:(id)value error:(NSError* _Nullable*)error;
- (NSArray*)getObject:(NSString*)entryName key:(NSString*)key lessThanOrEqualTo:(id)value error:(NSError* _Nullable*)error;
@end

NS_ASSUME_NONNULL_END
