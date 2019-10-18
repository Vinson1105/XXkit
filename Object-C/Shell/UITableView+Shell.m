//
//  UITableView+Shell.m
//  XXkitDemo
//
//  Created by VINSON on 2019/10/17.
//  Copyright © 2019 VINSON. All rights reserved.
//

#import "UITableView+Shell.h"
#import <objc/runtime.h>

#define OBJECTKEY @"XXshellData"

@interface UITableView()<UITableViewDataSource,UITableViewDelegate>
@property (nonatomic,strong) NSDictionary *shellData;
@end

@implementation UITableView (Shell)
#pragma mark - 公共接口: <初始化>
-(void)installShellWithString:(NSString*)json{
    self.delegate = self;
    self.dataSource = self;
}
-(void)installShellWithDictionary:(NSDictionary*)dict{
    self.delegate = self;
    self.dataSource = self;
    self.shellData = dict;
}

#pragma mark - 属性读写: <属性读/写>
- (void)setShellData:(NSDictionary *)shellData{
    objc_setAssociatedObject(self, OBJECTKEY, shellData, OBJC_ASSOCIATION_RETAIN_NONATOMIC);
}

#pragma mark - 协议函数: <UITableViewDataSource>
- (UITableViewCell *)cellForRowAtIndexPath:(NSIndexPath *)indexPath{
    return nil;
}

#pragma mark - 协议函数: <UITableViewDelegate>
@end

