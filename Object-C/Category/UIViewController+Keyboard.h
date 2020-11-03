//
//  UIViewController+Keyboard.h
//  XXkitProject
//
//  Created by VINSON on 2020/11/3.
//  Copyright © 2020 郭文轩. All rights reserved.
//

#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN

@interface UIViewController(Keyboard)
@property (nonatomic,assign) BOOL keyboard_tapToHidden;
-(void)keyboard_addFirstResponder:(id)responder;
-(void)keyboard_removeFirstResponder:(id)responder;
@end

NS_ASSUME_NONNULL_END
