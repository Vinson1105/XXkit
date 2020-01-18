//
//  EdgeInsetsLabel.m
//  iCamSee
//
//  Created by VINSON on 2019/11/29.
//  Copyright © 2019 Macrovideo. All rights reserved.
//

#import "EdgeInsetsLabel.h"

@implementation EdgeInsetsLabel
- (instancetype)initWithContentInsets:(UIEdgeInsets)contentInsets{
    self = [super init];
    if(self){
        _contentInsets = contentInsets;
    }
    return self;
}
- (void)drawTextInRect:(CGRect)rect{
    return [super drawTextInRect:UIEdgeInsetsInsetRect(rect,self.contentInsets)];
}
@end
