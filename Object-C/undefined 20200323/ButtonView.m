//
//  ButtonView.m
//  iCamSee
//
//  Created by VINSON on 2019/11/25.
//  Copyright © 2019 Macrovideo. All rights reserved.
//

#import "ButtonView.h"
#import "XXmacro.h"

@implementation ButtonView
- (instancetype)init{
    self = [super init];
    if (self) {
        _button = [UIButton buttonWithType:UIButtonTypeCustom];
        _button.translatesAutoresizingMaskIntoConstraints = NO;
        [self addSubview:_button];
        AnchorTop(_button, self, 0)
        AnchorCenterX(_button, self)
        
        _label = [UILabel new];
        _label.textAlignment = NSTextAlignmentCenter;
        _label.translatesAutoresizingMaskIntoConstraints = NO;
        [self addSubview:_label];
        [_label.topAnchor constraintEqualToAnchor:_button.bottomAnchor constant:2].active = YES;
        AnchorCenterX(_label, self)
    }
    return self;
}
@end
