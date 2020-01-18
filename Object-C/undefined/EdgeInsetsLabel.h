//
//  EdgeInsetsLabel.h
//  iCamSee
//
//  Created by VINSON on 2019/11/29.
//  Copyright © 2019 Macrovideo. All rights reserved.
//

#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN

@interface EdgeInsetsLabel : UILabel
@property (nonatomic,assign) UIEdgeInsets contentInsets;
-(instancetype)initWithContentInsets:(UIEdgeInsets)contentInsets;
@end

NS_ASSUME_NONNULL_END
