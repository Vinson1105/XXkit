//
//  RightBar.h
//  iCamSee
//
//  Created by VINSON on 2019/11/22.
//  Copyright © 2019 Macrovideo. All rights reserved.
//

#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN

@interface RightBar : UIView
@property (nonatomic,copy,nullable) void (^onClicked)(NSString *name, BOOL selected);

-(instancetype) initWithSuperview:(UIView*)view size:(CGSize)size spacing:(CGFloat)spacing padding:(CGFloat)padding margin:(CGFloat)margin;
-(void)addBarView:(UIView*)view name:(NSString*)name size:(CGSize)size;
-(UIButton*)addBarButton:(NSString*)name size:(CGSize)size;
-(UIButton*)addBarButton:(NSString*)name size:(CGSize)size normalImage:(NSString*)normal selectedImage:(nullable NSString*)selected disabledImage:(nullable NSString*)disabled;
-(void)removeBarView:(NSString*)name;
-(void)enabledBarView:(BOOL)enabled name:(NSString*)name;
@end

NS_ASSUME_NONNULL_END
