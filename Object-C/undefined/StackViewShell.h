//
//  StackViewShell.h
//  iCamSee
//
//  Created by VINSON on 2019/11/19.
//  Copyright © 2019 Macrovideo. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN

@interface StackViewShell : NSObject
@property (nonatomic,weak,readonly) UIStackView *target;

-(void)shell:(UIStackView*)target;
-(void)add:(NSString*)name view:(UIView*)view;
-(void)add:(NSString*)name size:(CGSize)size view:(UIView*)view;
-(void)remove:(NSString*)name;
-(void)removeAll;
-(void)insert:(NSString*)name view:(UIView*)view atIndex:(int)index;
-(void)insert:(NSString*)name size:(CGSize)size view:(UIView*)view atIndex:(int)index;
-(void)hide:(BOOL)hide Name:(NSString*)name;
-(nullable UIView*)view:(NSString*)name;
@end

NS_ASSUME_NONNULL_END
