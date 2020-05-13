//
//  XXcoreData.h
//  XXkitProject
//
//  Created by VINSON on 2020/5/13.
//  Copyright © 2020 郭文轩. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <CoreData/CoreData.h>

NS_ASSUME_NONNULL_BEGIN

@interface XXcoreData : NSObject
@property (nonatomic,strong,readonly) NSManagedObjectContext *context;
+ (XXcoreData*)sharedInstance;
- (void)configModel:(NSString*)name bundle:(nullable NSBundle*)bundle storeType:(NSString*)storeType storeUrl:(NSURL*)storeUrl;

- (void)insertObject:(NSString*)entryName initHandle:(void(^)(id obj))initHandle error:(NSError* _Nullable*)error;

- (void)deleteObject:(NSString*)entryName predicate:(NSPredicate*)predicate error:(NSError* _Nullable*)error;
- (void)deleteObject:(NSString*)entryName key:(NSString*)key equelTo:(id)value error:(NSError* _Nullable*)error;
- (void)deleteObject:(NSString*)entryName key:(NSString*)key greaterThanTo:(id)value error:(NSError* _Nullable*)error;
- (void)deleteObject:(NSString*)entryName key:(NSString*)key greaterThanOrEqualTo:(id)value error:(NSError* _Nullable*)error;
- (void)deleteObject:(NSString*)entryName key:(NSString*)key lessThanTo:(id)value error:(NSError* _Nullable*)error;
- (void)deleteObject:(NSString*)entryName key:(NSString*)key lessThanOrEqualTo:(id)value error:(NSError* _Nullable*)error;

- (NSArray*)getObject:(NSString*)entryName predicate:(NSPredicate*)predicate error:(NSError* _Nullable*)error;
- (NSArray*)getObject:(NSString*)entryName key:(NSString*)key equelTo:(id)value error:(NSError* _Nullable*)error;
- (NSArray*)getObject:(NSString*)entryName key:(NSString*)key greaterThanTo:(id)value error:(NSError* _Nullable*)error;
- (NSArray*)getObject:(NSString*)entryName key:(NSString*)key greaterThanOrEqualTo:(id)value error:(NSError* _Nullable*)error;
- (NSArray*)getObject:(NSString*)entryName key:(NSString*)key lessThanTo:(id)value error:(NSError* _Nullable*)error;
- (NSArray*)getObject:(NSString*)entryName key:(NSString*)key lessThanOrEqualTo:(id)value error:(NSError* _Nullable*)error;
@end

NS_ASSUME_NONNULL_END
