//
//  TopTitleView.h
//  iCamSee
//
//  Created by VINSON on 2019/11/21.
//  Copyright © 2019 Macrovideo. All rights reserved.
//

#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN

@interface TopBar : UIView
@property (nonatomic,copy) NSString *leftTitle;
@property (nonatomic,copy) NSString *midTitle;
@property (nonatomic,copy,nullable) void(^onReturn)(void);
-(instancetype) initWithSuperview:(UIView*)view size:(CGSize)size returnImage:(UIImage*)image margin:(CGFloat)margin;
@end

NS_ASSUME_NONNULL_END
