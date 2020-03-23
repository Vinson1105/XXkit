//
//  XXmacro.h
//  iCamSee
//
//  Created by VINSON on 2019/11/21.
//  Copyright © 2019 Macrovideo. All rights reserved.
//

#ifndef XXmacro_h
#define XXmacro_h

#define ButtonTitleAll(button,normal,selected,disnorm,dissel)                       \
    [button setTitle:normal forState:UIControlStateNormal];                         \
    [button setTitle:disnorm forState:UIControlStateNormal|UIControlStateDisabled]; \
    [button setTitle:selected forState:UIControlStateSelected];                     \
    [button setTitle:dissel forState:UIControlStateSelected|UIControlStateDisabled]; \

#define ButtonImageAll(button,normal,selected,disnorm,dissel) \
    [button setImage:[UIImage imageNamed:normal] forState:UIControlStateNormal];   \
    [button setImage:[UIImage imageNamed:disnorm] forState:UIControlStateNormal|UIControlStateDisabled]; \
    [button setImage:[UIImage imageNamed:selected] forState:UIControlStateSelected]; \
    [button setImage:[UIImage imageNamed:dissel] forState:UIControlStateSelected|UIControlStateDisabled];

#define ButtonImageNSD(button,normal,selected,disabled) \
    [button setImage:[UIImage imageNamed:normal] forState:UIControlStateNormal];   \
    [button setImage:[UIImage imageNamed:selected] forState:UIControlStateSelected]; \
    [button setImage:[UIImage imageNamed:disabled] forState:UIControlStateDisabled];

#define ButtonImageND(button,normal,disabled) \
    [button setImage:[UIImage imageNamed:normal] forState:UIControlStateNormal];   \
    [button setImage:[UIImage imageNamed:disabled] forState:UIControlStateDisabled];

#define ButtonImageNS(button,normal,selected) \
    [button setImage:[UIImage imageNamed:normal] forState:UIControlStateNormal];   \
    [button setImage:[UIImage imageNamed:selected] forState:UIControlStateSelected];

#define ButtonImageNHD(button,normal,highlight,disabled) \
    [button setImage:[UIImage imageNamed:normal] forState:UIControlStateNormal];   \
    [button setImage:[UIImage imageNamed:highlight] forState:UIControlStateHighlighted]; \
    [button setImage:[UIImage imageNamed:disabled] forState:UIControlStateDisabled];

#define SET_BUTTON_IMAGE(button,normal,highlighted,selected,disabled) \
    [button setImage:[UIImage imageNamed:normal] forState:UIControlStateNormal];   \
    if(nil != highlight) [button setImage:[UIImage imageNamed:highlight] forState:UIControlStateHighlighted]; \
    if(nil != selected) [button setImage:[UIImage imageNamed:selected] forState:UIControlStateSelected]; \
    if(nil != disabled) [button setImage:[UIImage imageNamed:disabled] forState:UIControlStateDisabled];

#define ButtonColorND(button,normal,disabled) \
    [button setTitleColor:[UIColor colorWithHexString:normal] forState:UIControlStateNormal];       \
    [button setTitleColor:[UIColor colorWithHexString:disabled] forState:UIControlStateDisabled];

#define AnchorFillSave(array,item,to)                                                   \
    NSMutableArray *array = [NSMutableArray new];                                       \
    [array addObject:[item.topAnchor constraintEqualToAnchor:to.topAnchor]];            \
    [array addObject:[item.leadingAnchor constraintEqualToAnchor:to.leadingAnchor]];    \
    [array addObject:[item.bottomAnchor constraintEqualToAnchor:to.bottomAnchor]];      \
    [array addObject:[item.trailingAnchor constraintEqualToAnchor:to.trailingAnchor]];  \

#define AnchorTop(item,to,c)        [item.topAnchor constraintEqualToAnchor:to.topAnchor constant:c].active=YES;
#define AnchorLeading(item,to,c)    [item.leadingAnchor constraintEqualToAnchor:to.leadingAnchor constant:c].active=YES;
#define AnchorBottom(item,to,c)     [item.bottomAnchor constraintEqualToAnchor:to.bottomAnchor constant:c].active=YES;
#define AnchorTrailing(item,to,c)   [item.trailingAnchor constraintEqualToAnchor:to.trailingAnchor constant:c].active=YES;

#define AnchorFill(item,to,top,leading,bottom,trailing) \
    AnchorTop(item,to,top) AnchorLeading(item,to,leading) AnchorBottom(item,to,bottom) AnchorTrailing(item,to,trailing)

#define AnchorCenterX(item,to) [item.centerXAnchor constraintEqualToAnchor:to.centerXAnchor].active = YES;
#define AnchorCenterY(item,to) [item.centerYAnchor constraintEqualToAnchor:to.centerYAnchor].active = YES;
#define AnchorCenter(item,to) \
    AnchorCenterX(item,to) \
    AnchorCenterY(item,to)

#define AnchorWidth(item,width) [item.widthAnchor constraintEqualToConstant:width].active = YES;
#define AnchorHeight(item,height) [item.heightAnchor constraintEqualToConstant:height].active = YES;
#define AnchorSize(item,width,height) \
    AnchorWidth(item,width) \
    AnchorHeight(item,height)

#define WeakSelf        __weak typeof(self) weakSelf = self;
#define StrongSelf      __strong typeof(weakSelf) strongSelf = weakSelf;
#define Weak(w,src)     __weak typeof(src) w = src;
#define Strong(s,src)   __strong typeof(src) s = src;

#endif /* XXmacro_h */
