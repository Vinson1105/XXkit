//
//  ButtonPressShell.h
//  iCamSee
//
//  Created by VINSON on 2020/3/23.
//  Copyright © 2020 Macrovideo. All rights reserved.
//

#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN

@interface ButtonPressShell : NSObject
@property (nonatomic,copy) void(^onBegin)(void);
@property (nonatomic,copy) void(^onPressed)(NSArray *names);
@property (nonatomic,copy) void(^onEnd)(void);
@property (nonatomic,assign) CGFloat interval;  // 默认0.2s
- (BOOL)addButton:(UIButton*)button withName:(NSString*)name;
- (void)removeButtonForName:(NSString*)name;
- (void)stop;
@end

NS_ASSUME_NONNULL_END
