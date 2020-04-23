//
//  XXloginShell.h
//  XXkitProject
//
//  Created by VINSON on 2020/4/23.
//  Copyright © 2020 郭文轩. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "../UITextField/XXtextFieldShell.h"

NS_ASSUME_NONNULL_BEGIN

@interface XXloginShell : NSObject
@property (nonatomic,strong,readonly) XXtextFieldShell *accountShell;
@property (nonatomic,strong,readonly) XXtextFieldShell *passwordShell;

- (void)shellWithAccount:(UITextField*)account password:(UITextField*)password;

- (void)configAccountWithClearButton:(UIButton*)clearButton;
- (void)configPasswordWithClearButton:(UIButton*)clearButton secureButton:(UIButton*)secureButton secureON:(UIImage*)secureON secureOFF:(UIImage*)secureOFF;

- (void)configAccountWithLogoImage:(nullable UIImage*)logoImage clearImage:(UIImage*)clearImage size:(CGSize)size;
- (void)configPasswordWithLogoImage:(nullable UIImage*)logoImage
                         ClearImage:(UIButton*)clearImage
                           secureON:(UIImage*)secureON
                          secureOFF:(UIImage*)secureOFF
                               size:(CGSize)size
                             margin:(CGFloat)margin
                            spacing:(CGFloat)spacing;

- (void)configOKWithBlock:(void(^)(NSString *,NSString *))onOK;
@end

NS_ASSUME_NONNULL_END
