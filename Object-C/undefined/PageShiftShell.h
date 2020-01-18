//
//  ViewPageShiftShell.h
//  iCamSee
//
//  Created by VINSON on 2019/11/19.
//  Copyright © 2019 Macrovideo. All rights reserved.
//

#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN

@interface PageShiftShell : NSObject
@property (nonatomic,weak,readonly) UIButton *leftButton;
@property (nonatomic,weak,readonly) UIButton *rightButton;
@property (nonatomic,weak,readonly) UILabel *pageLabel;
@property (nonatomic,copy,nullable) void(^onPageChanged)(int previous, int current);

@property(nonatomic,assign) int currentPage;            // 当前页
@property(nonatomic,assign) int pageCount;              // 页数量
@property (nonatomic,assign) BOOL pageBeginFromZore;    // 页标签是否从0开始显示,如0/8,1/8..7/8; 默认是NO,即从1开始显示,如1/8,2/8...8/8

-(void)shellWithLeft:(UIButton*)left Right:(UIButton*)right Page:(UILabel*)page;
@end

NS_ASSUME_NONNULL_END
