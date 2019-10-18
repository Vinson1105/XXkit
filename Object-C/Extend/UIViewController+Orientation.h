//
//  UIViewController+Orientation.h
//  Bolts
//
//  Created by VINSON on 2019/10/14.
//

#import <UIKit/UIKit.h>
NS_ASSUME_NONNULL_BEGIN

@interface UIViewController (Orientation)
@property (nonatomic,assign) BOOL landscape;
@property (nonatomic,copy,nullable) void(^landscapeChangedBlock)(BOOL onLandscape);
- (void) installOrientation:(BOOL)install;
@end

NS_ASSUME_NONNULL_END
