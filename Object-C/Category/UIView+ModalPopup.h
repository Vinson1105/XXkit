//
//  UIView+ModalPopup.h
//  XXkitProject
//
//  Created by 郭文轩 on 2020/11/16.
//  Copyright © 2020 郭文轩. All rights reserved.
//

#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN

typedef enum : NSUInteger {
    ModalPopupPositionCenter,
    ModalPopupPositionBottom,
} ModalPopupPosition;

@interface UIView(ModalPopup)
@property (nonatomic,strong,readonly) UIView *modalPopup_backgroundView;
@property (nonatomic,assign) BOOL modalPopup_up;
-(void)modalPopup_installWithPosition:(ModalPopupPosition)position;
-(void)modalPopup_uninstall;
@end

NS_ASSUME_NONNULL_END
