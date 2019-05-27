//
//  NSFileHandle+XXextend.h
//  XXapp
//
//  Created by VINSON on 2019/4/22.
//  Copyright © 2019 VINSON. All rights reserved.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@interface NSFileHandle(XXextend)
+ (instancetype)fileHandleForWritingInDocumentAtFileName:(NSString *)fileName;
+ (instancetype)fileHandleForWritingInDocumentAtFilePath:(NSArray *)filePathNode;
@end

NS_ASSUME_NONNULL_END
