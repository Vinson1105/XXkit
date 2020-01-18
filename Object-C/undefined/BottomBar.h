//
//  BottomBar.h
//  iCamSee
//
//  Created by VINSON on 2019/11/21.
//  Copyright © 2019 Macrovideo. All rights reserved.
//

#import <UIKit/UIKit.h>

typedef enum : NSUInteger {
    BottomBarPositionLeft,
    BottomBarPositionRight,
} BottomBarPosition;

NS_ASSUME_NONNULL_BEGIN

@interface BottomBar : UIView
@property (nonatomic,copy,nullable) void (^onClicked)(NSString *name, BOOL selected);

-(instancetype) initWithSuperview:(UIView*)view size:(CGSize)size spacing:(CGFloat)spacing padding:(CGFloat)padding;
-(void)addBarView:(UIView*)view name:(NSString*)name atPosition:(BottomBarPosition)position;
-(void)addBarView:(UIView*)view name:(NSString*)name size:(CGSize)size atPosition:(BottomBarPosition)position;
-(UIButton*)addBarButton:(NSString*)name normalImage:(nullable NSString*)normal selectedImage:(nullable NSString*)selected disabledImage:(nullable NSString*)disabled autoSelect:(BOOL)autoSelect atPosition:(BottomBarPosition)position;
-(UIButton*)addBarButton:(NSString*)name size:(CGSize)size normalImage:(nullable NSString*)normal selectedImage:(nullable NSString*)selected disabledImage:(nullable NSString*)disabled atPosition:(BottomBarPosition)position;
-(void)remove:(NSString*)name atPosition:(BottomBarPosition)position;
-(void)enabled:(BOOL)enabled name:(NSString*)name atPosition:(BottomBarPosition)position;
-(void)enabled:(BOOL)enabled names:(NSArray*)names atPosition:(BottomBarPosition)position;

-(UIView*)barView:(NSString*)name atPosition:(BottomBarPosition)position;
-(UIButton*)barButton:(NSString*)name atPosition:(BottomBarPosition)position;
@end

NS_ASSUME_NONNULL_END
