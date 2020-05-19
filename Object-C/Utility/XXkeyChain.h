//
//  XXkeyChain.h
//  XXkitProject
//
//  Created by 郭文轩 on 2020/5/19.
//  Copyright © 2020 郭文轩. All rights reserved.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@interface XXkeyChain : NSObject
@property (nonatomic,copy) NSString *service;
+ (XXkeyChain*)sharedInstance;
- (void)saveAccount:(NSString*)account password:(NSString *)password error:(NSError * _Nullable *)error;
- (void)deleteAccount:(NSString*)account error:(NSError * _Nullable *)error;
- (NSString*)getPasswordWithAccount:(NSString*)account error:(NSError * _Nullable *)error;
- (void)updateAccount:(NSString *)account password:(NSString*)password error:(NSError * _Nullable *)error;
- (BOOL)isExist:(NSString*)account error:(NSError * _Nullable *)error;
@end

NS_ASSUME_NONNULL_END
