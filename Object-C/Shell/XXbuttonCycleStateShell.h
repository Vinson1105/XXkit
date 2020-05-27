//
//  XXloopButtonShell.h
//  iCamSee
//
//  Created by VINSON on 2020/5/11.
//  Copyright © 2020 Macrovideo. All rights reserved.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@interface XXbuttonCycleStateShell : NSObject
@property (nonatomic,weak,readonly) UIButton *button;
@property (nonatomic,copy) void(^onCycleStateChanged)(XXbuttonCycleStateShell *shell, NSString *previous, NSString *current);
- (void)shell:(UIButton*)button;
- (void)configCycleState:(NSString*)key image:(UIImage*)image;
- (void)resetCycleState:(NSString*)key;
@end

NS_ASSUME_NONNULL_END
