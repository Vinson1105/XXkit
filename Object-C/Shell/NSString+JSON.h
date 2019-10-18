//
//  NSString+JSON.h
//  XXkitDemo
//
//  Created by VINSON on 2019/10/18.
//  Copyright © 2019 VINSON. All rights reserved.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@interface NSString (JSON)
+(NSString*)jsonStringFromJSONObject:(id)jsonObject;
-(id)jsonObject;
@end

NS_ASSUME_NONNULL_END
