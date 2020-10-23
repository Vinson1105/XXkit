//
//  QuickComponentUINavigationItem.m
//  XXkitProject
//
//  Created by VINSON on 2020/10/22.
//  Copyright © 2020 郭文轩. All rights reserved.
//

#import "QuickComponentUINavigationItem.h"
#import <UIKit/UIKit.h>

static NSString * const kLeftItem = @"LeftItem";
static NSString * const kRightItem = @"RightItem";

static NSString * const kImage = @"Image";
static NSString * const kAction = @"Action";

@implementation QuickComponentUINavigationItem
+ (NSString *)targetClass{
    return NSStringFromClass(UINavigationItem.class);
}
+ (void)obj:(id)obj key:(NSString *)key value:(id)value{
    // MARK: LeftItem || RightItem
    if([key isEqualToString:kLeftItem] || [key isEqualToString:kRightItem]){
//        if([value isKindOfClass:NSDictionary.class]){
//            NSDictionary *dict = value;
//            if(dict[kImage] && dict[kAction]){
//                UIBarButtonItem *item = [UIBarButtonItem alloc] initWithImage:[dict[kImage] isKindOfClass:UIImage.class]?dict[kImage]:[UIImage imageNamed:dict[kImage]] style:UIBarButtonItemStylePlain target: action:<#(nullable SEL)#>
//            }
//            else{
//                [super obj:obj key:key value:value];
//            }
//        }
//        else{
//            [super obj:obj key:key value:value];
//        }
        [super obj:obj key:key value:value];

    }
    
    else {
        [super obj:obj key:key value:value];
    }
}
@end
