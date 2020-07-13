//
//  XXviewMovableShell.h
//  XXkitProject
//
//  Created by VINSON on 2020/7/13.
//  Copyright © 2020 郭文轩. All rights reserved.
//

#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN

@interface XXviewMovableShell : NSObject
@property (nonatomic,strong,readonly) UIView *view;
-(void)shell:(UIView*)view;
@end

NS_ASSUME_NONNULL_END
