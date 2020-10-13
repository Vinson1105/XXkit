//
//  NSString+JSON.h
//  XXkitProject
//
//  Created by 郭文轩 on 2020/10/12.
//  Copyright © 2020 郭文轩. All rights reserved.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@interface NSString(JSON)
-(id)toJsonObject:(NSError * _Nullable *)error;
-(id)toMutableJsonObject:(NSError * _Nullable *)error;
@end

NS_ASSUME_NONNULL_END
