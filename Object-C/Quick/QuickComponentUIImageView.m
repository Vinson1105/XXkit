//
//  QuickComponentUIImageView.m
//  XXkitProject
//
//  Created by VINSON on 2020/11/2.
//  Copyright © 2020 郭文轩. All rights reserved.
//

#import "QuickComponentUIImageView.h"
#import <UIKit/UIKit.h>
#import "../XXocUtils.h"

static NSString * const kImage = @"Image";

@implementation QuickComponentUIImageView
+ (NSString *)targetClass{
    return NSStringFromClass(UIImageView.class);
}
+ (void)obj:(id)obj key:(NSString *)key value:(id)value{
    UIImageView *imageView = obj;
    
    // MARK: Image
    if([key isEqualToString:kImage]){
        imageView.image = [XXocUtils autoImage:value];
    }
    else{
        [super obj:obj key:key value:value];
    }
}
@end
