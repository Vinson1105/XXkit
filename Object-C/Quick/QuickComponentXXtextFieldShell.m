//
//  QuickComponentXXtextFieldShell.m
//  XXkitProject
//
//  Created by VINSON on 2020/10/24.
//  Copyright © 2020 郭文轩. All rights reserved.
//

#import "QuickComponentXXtextFieldShell.h"
#import "../Shell/XXtextFieldShell.h"

static NSString * const kShell = @"Shell";
static NSString * const kExpression = @"Expression";
static NSString * const kStrongMatchMode = @"StrongMatchMode";
static NSString * const kMaxLength = @"MaxLength";

static NSString * const kLeft = @"Left";
    static NSString * const kImage = @"Image";
    static NSString * const kSize=  @"Size";
static NSString * const kRight = @"Right";
    static NSString * const kClearImage = @"ClearImage";
    static NSString * const kSecureON = @"SecureON";
    static NSString * const kSecureOFF = @"SecureOFF";
    static NSString * const kMargin = @"Margin";
    static NSString * const kSpacing = @"Spacing";
    // size

@implementation QuickComponentXXtextFieldShell
+ (NSString *)targetClass{
    return NSStringFromClass(XXtextFieldShell.class);
}
+ (void)obj:(id)obj key:(NSString *)key value:(id)value{
    XXtextFieldShell *shell = obj;
    
    // MARK: Shell
    if([key isEqualToString:kShell]){
        
    }
    
    // MARK: Expression
    else if(key isEqualToString:k){
        
    }
    
    else if([key isEqualToString:kLeft]){
        
    }
    else if([key isEqualToString:kRight]){
        
    }
    else{
        [super obj:obj key:key value:value];
    }
}
@end
