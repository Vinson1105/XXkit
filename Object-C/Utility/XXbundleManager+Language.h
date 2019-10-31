//
//  XXbundleManager+Language.h
//  XXkitDemo
//
//  Created by VINSON on 2019/10/30.
//  Copyright © 2019 VINSON. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "XXbundleManager.h"

NS_ASSUME_NONNULL_BEGIN

@interface XXbundleManager(Language)
+(nullable NSArray*)availableLanguage;
+(nullable NSString*)currentLanguage;
+(void)setLanguage:(NSString*)language;
@end

NS_ASSUME_NONNULL_END
