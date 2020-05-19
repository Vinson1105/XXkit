//
//  XXkeyChain.m
//  XXkitProject
//
//  Created by 郭文轩 on 2020/5/19.
//  Copyright © 2020 郭文轩. All rights reserved.
//

#import "XXkeyChain.h"
#import <Security/SecItem.h>
#import <Security/SecBase.h>

#define kService @"XXkeyChain"
static NSInteger const kErrorCodeKeychainSomeArgumentsInvalid = 1000;
static XXkeyChain *_instance = nil;

@implementation XXkeyChain
+ (XXkeyChain*)sharedInstance{
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        _instance = [XXkeyChain new];
        _instance.service = kService;
    });
    return _instance;
}
- (void)saveAccount:(NSString*)account password:(NSString *)password error:(NSError * _Nullable __autoreleasing * _Nullable)error{
    if (!account || !password || !_service) {
        if(error){ *error = [self toError:kErrorCodeKeychainSomeArgumentsInvalid]; }
        return;
    }
    
    NSDictionary *queryItems = @{
        (id)kSecClass: (id)kSecClassGenericPassword,
        (id)kSecAttrService: _service,
        (id)kSecAttrAccount: account
    };
    
    NSData *passwordData        = [password dataUsingEncoding:NSUTF8StringEncoding];
    NSDictionary *passwordItem  = @{(id)kSecValueData: passwordData,};
    
    OSStatus status = SecItemUpdate((CFDictionaryRef)queryItems, (CFDictionaryRef)passwordItem);
    if(status != errSecSuccess && error){
        *error = [self toError:status];
    }
}
- (void)deleteAccount:(NSString*)account error:(NSError * _Nullable __autoreleasing * _Nullable)error{
    if (!_service || !account) {
        if(error){ *error = [self toError:kErrorCodeKeychainSomeArgumentsInvalid]; }
        return;
    }
    NSDictionary *deleteSecItems = @{
        (id)kSecClass: (id)kSecClassGenericPassword,
        (id)kSecAttrService: _service,
        (id)kSecAttrAccount: account
    };
    OSStatus status = SecItemDelete((CFDictionaryRef)deleteSecItems);
    if(error){
        *error = [self toError:status];
    }
}
- (NSString*)getPasswordWithAccount:(NSString*)account error:(NSError * _Nullable __autoreleasing * _Nullable)error{
    if (!_service || !account) {
        if(error){ *error = [self toError:kErrorCodeKeychainSomeArgumentsInvalid]; }
        return nil;
    }
    
    NSDictionary *queryItems = @{
        (id)kSecClass: (id)kSecClassGenericPassword,
        (id)kSecAttrService: _service,
        (id)kSecAttrAccount: account,
        (id)kSecMatchLimit: (id)kSecMatchLimitOne,
        (id)kSecReturnData: @(YES)
    };
    
    NSString *password  = nil;
    CFTypeRef dataRef   = nil;
    OSStatus status     = SecItemCopyMatching((CFDictionaryRef)queryItems, (CFTypeRef *)&dataRef);
    if (status == errSecSuccess) {
        password = [[NSString alloc] initWithData:CFBridgingRelease(dataRef) encoding:NSUTF8StringEncoding];
    }
    if (error) {
        *error = [self toError:status];
    }
    return password;
}
- (void)updateAccount:(NSString *)account password:(NSString*)password error:(NSError * _Nullable __autoreleasing * _Nullable)error{
    if (!account || !password || !_service) {
        if(error){ *error = [self toError:kErrorCodeKeychainSomeArgumentsInvalid]; }
        return;
    }
    
    if ([self isExist:account error:nil]) {
        [self updateAccount:account password:password error:error];
    }
    else{
        NSData *passwordData        = [password dataUsingEncoding:NSUTF8StringEncoding];
        NSDictionary *saveSecItems  = @{
            (id)kSecClass: (id)kSecClassGenericPassword,
            (id)kSecAttrService: _service,
            (id)kSecAttrAccount: account,
            (id)kSecValueData: passwordData
        };
        
        OSStatus status = SecItemAdd((CFDictionaryRef)saveSecItems, NULL);
        if(error){
            *error = [self toError:status];
        }
    }
}
- (BOOL)isExist:(NSString*)account error:(NSError * _Nullable __autoreleasing * _Nullable)error{
    if (!account || !_service) {
        if(error){ *error = [self toError:kErrorCodeKeychainSomeArgumentsInvalid]; }
        return NO;
    }
    
    NSDictionary *queryItems = @{
        (id)kSecClass: (id)kSecClassGenericPassword,
        (id)kSecAttrService: _service,
        (id)kSecAttrAccount: account,
        (id)kSecMatchLimit: (id)kSecMatchLimitOne,
        (id)kSecReturnData: @(YES)
    };
    
    OSStatus status = SecItemCopyMatching((CFDictionaryRef)queryItems, NULL);
    if(error){ *error = [self toError:kErrorCodeKeychainSomeArgumentsInvalid]; }
    return status == errSecSuccess;
}

- (NSError*)toError:(OSStatus)status{
    NSString *errorString;
    if(kErrorCodeKeychainSomeArgumentsInvalid == status){
        errorString = @"argument is not valid.";
    }
    else if(errSecSuccess == status){
        return nil;
    }
    else{
        errorString = CFBridgingRelease(SecCopyErrorMessageString(status, NULL));
    }
    return [NSError errorWithDomain:@"XXkeyChain" code:status userInfo:@{NSLocalizedDescriptionKey:errorString}];
}
@end
