//
//  ButtonShell.h
//  iCamSee
//
//  Created by VINSON on 2019/11/19.
//  Copyright © 2019 Macrovideo. All rights reserved.
//

#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN


@interface ButtonSelectShell : NSObject
@property (nonatomic,assign) BOOL exclusive;
@property (nonatomic,copy,nullable) void (^onClicked)(NSString *name, BOOL selected);

-(BOOL)addButton:(UIButton*)button name:(NSString*)name;
-(BOOL)addSelectableButton:(UIButton*)button name:(NSString*)name;
-(void)remove:(NSString*)name;
-(void)removeAll;
-(void)selected:(BOOL)selected name:(NSString*)name;
-(void)selected:(BOOL)selected name:(NSString*)name update:(BOOL)update;
-(void)enabled:(BOOL)enabled name:(NSString*)name;
-(void)enabled:(BOOL)enabled names:(NSArray*)names;
-(void)hidden:(BOOL)hidden name:(NSString*)name;
@end

NS_ASSUME_NONNULL_END
