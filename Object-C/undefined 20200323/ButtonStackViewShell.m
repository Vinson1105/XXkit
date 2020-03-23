//
//  StackViewShell+Button.m
//  iCamSee
//
//  Created by VINSON on 2019/11/19.
//  Copyright © 2019 Macrovideo. All rights reserved.
//

#import "ButtonStackViewShell.h"
#import <objc/runtime.h>
#import "XXmacro.h"

@interface ButtonStackViewShell()
@end

@implementation ButtonStackViewShell
- (void)shell:(UIStackView *)target{
    [super shell:target];
    _selectShell = [ButtonSelectShell new];
}
- (void)setOnClicked:(void (^)(NSString * _Nonnull, BOOL))onClicked{
    _selectShell.onClicked = onClicked;
}
- (void (^)(NSString * _Nonnull, BOOL))onClicked{
    return _selectShell.onClicked;
}

-(UIButton*)addButton:(NSString*)name size:(CGSize)size selectable:(BOOL)selectable{
    UIButton *button = [UIButton buttonWithType:UIButtonTypeCustom];
    [self add:name size:size view:button];
    if(selectable)
        [_selectShell addSelectableButton:button name:name];
    else
        [_selectShell addButton:button name:name];
    return button;
}
-(UIButton*)insertButton:(NSString*)name size:(CGSize)size selectable:(BOOL)selectable atIndex:(int)index{
    UIButton *button = [UIButton buttonWithType:UIButtonTypeCustom];
    [self insert:name size:size view:button atIndex:index];
    if(selectable)
        [_selectShell addSelectableButton:button name:name];
    else
        [_selectShell addButton:button name:name];
    return button;
}
@end
