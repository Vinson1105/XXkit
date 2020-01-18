//
//  UIButton+XX.h
//  Bolts
//
//  Created by VINSON on 2019/10/16.
//

#import <UIKit/UIKit.h>
NS_ASSUME_NONNULL_BEGIN

@interface UIButton(XX)
@property (nonatomic,copy,readonly) UIButton*(^normalTitle)(NSString *t);
@property (nonatomic,copy,readonly) UIButton*(^normalTitleColor)(UIColor *c);
@property (nonatomic,copy,readonly) UIButton*(^selectedTitle)(NSString *t);
@property (nonatomic,copy,readonly) UIButton*(^radius)(CGFloat r);
@property (nonatomic,copy,readonly) UIButton*(^borderWidth)(CGFloat w);
@property (nonatomic,copy,readonly) UIButton*(^borderColor)(UIColor *c);
@end

NS_ASSUME_NONNULL_END
