//
//  StackViewShell+Button.h
//  iCamSee
//
//  Created by VINSON on 2019/11/19.
//  Copyright © 2019 Macrovideo. All rights reserved.
//

#import "StackViewShell.h"
#import "ButtonSelectShell.h"

NS_ASSUME_NONNULL_BEGIN

@interface ButtonStackViewShell : StackViewShell
@property (nonatomic,strong) ButtonSelectShell *selectShell;

@property (nonatomic,copy,nullable) void(^onClicked)(NSString *name, BOOL isSelected);
-(UIButton*)addButton:(NSString*)name size:(CGSize)size selectable:(BOOL)selectable;
-(UIButton*)insertButton:(NSString*)name size:(CGSize)size selectable:(BOOL)selectable atIndex:(int)index;
@end

NS_ASSUME_NONNULL_END
