//
//  XXtextFieldShell.h
//  XXkitProject
//
//  Created by VINSON on 2020/4/22.
//  Copyright © 2020 郭文轩. All rights reserved.
//

#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN

typedef NS_ENUM(NSUInteger, XXtextFieldShellState) {
    XXtextFieldShellStateEditBegin,
    XXtextFieldShellStateEditEnd,
};

@interface XXtextFieldShell : NSObject
@property (nonatomic,readonly,weak) UITextField *target;
@property (nonatomic,copy,nullable) NSString *express;
@property (nonatomic,copy,nullable) void(^onStateChanged)(XXtextFieldShell *shell,)
- (void)shell:(UITextField*)target;
- (void)configImage:(UIImage*)image mode:(UITextFieldViewMode)mode left:(BOOL)left;
@end

NS_ASSUME_NONNULL_END
