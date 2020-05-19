/**
 2020.05.19
 */
#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@interface XXkeyChain : NSObject
/** 钥匙串访问标识 */
@property (nonatomic,copy) NSString *service;

+ (XXkeyChain*)sharedInstance;

/**
 删除账号
 @param account 目标账号
 @param error 错误返回
 */
- (void)deleteAccount:(NSString*)account error:(NSError * _Nullable *)error;

/**
 获取对应账号的密码
 @param account 目标账号
 @param error 错误返回
 @return 对应的密码，出错返回nil
 */
- (NSString*)getPasswordWithAccount:(NSString*)account error:(NSError * _Nullable *)error;

/**
 更新账号密码
 @param account 目标账号
 @param password 密码
 @param error 错误返回
 */
- (void)updateAccount:(NSString *)account password:(NSString*)password error:(NSError * _Nullable *)error;

/**
 查询账号数据是否存在
 @param account 目标账号
 */
- (BOOL)isExist:(NSString*)account error:(NSError * _Nullable *)error;
@end

NS_ASSUME_NONNULL_END
