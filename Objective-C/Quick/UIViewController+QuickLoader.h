//
//  UIViewController+QuickLoader.h
//  XXkitProject
//
//  Created by 郭文轩 on 2020/10/13.
//  Copyright © 2020 郭文轩. All rights reserved.
//

#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN

@interface UIViewController(QuickLoader)
-(void)quick_loadFromFileName:(NSString*)fileName bundle:(nullable NSBundle*)bundle;
-(void)quick_loadFromData:(NSData*)data;
@end

NS_ASSUME_NONNULL_END
