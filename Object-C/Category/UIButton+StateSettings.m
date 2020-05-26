//
//  UIButton+XX.m
//  Bolts
//
//  Created by VINSON on 2019/10/16.
//

#import "UIButton+StateSettings.h"
#import <objc/runtime.h>
#import "../XXocUtils.h"

#define kStateToSettings    @"StateSettings_stateToSettingsMap"
#define kBorderColor        @"BorderColor"
#define kBackgroundColor    @"BackgroundColor"

@interface UIButton()
@property (nonatomic,strong) NSMutableDictionary *stateToSettings;
@end

@implementation UIButton(StateSettings)
+ (void)load{
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        //[XXocUtils replaceMethod:[self class] src:@selector(setState:) dest:@selector(xx_setState:)];
        [XXocUtils replaceMethod:[self class] src:@selector(setSelected:) dest:@selector(xx_setSelected:)];
    });
}

- (void)setStateToSettings:(NSMutableDictionary *)stateToSettings{
    objc_setAssociatedObject(self, kStateToSettings, stateToSettings, OBJC_ASSOCIATION_RETAIN_NONATOMIC);
}
- (NSMutableDictionary *)stateToSettings{
    NSMutableDictionary *dict = objc_getAssociatedObject(self, kStateToSettings);
    if(!dict){
        dict = [NSMutableDictionary new];
        objc_setAssociatedObject(self, kStateToSettings, dict, OBJC_ASSOCIATION_RETAIN_NONATOMIC);
    }
    return dict;
}

- (void)setBorderColor:(UIColor *)color forState:(UIControlState)state{
    NSMutableDictionary *allSettings = self.stateToSettings;
    NSMutableDictionary *settings = allSettings[@(state)];
    if(nil == settings){
        settings = [NSMutableDictionary new];
        allSettings[@(state)] = settings;
    }
    settings[kBorderColor] = color;
    if(self.state == state){
        self.layer.borderColor = color.CGColor;
    }
}
- (void)setBackgroundColor:(UIColor *)color forState:(UIControlState)state{
    NSMutableDictionary *allSettings = self.stateToSettings;
    NSMutableDictionary *settings = allSettings[@(state)];
    if(nil == settings){
        settings = [NSMutableDictionary new];
        allSettings[@(state)] = settings;
    }
    settings[kBackgroundColor] = color;
    if(self.state == state){
        self.backgroundColor = color;
    }
}

- (void)xx_setSelected:(BOOL)selected{
    [self xx_setSelected:selected];
    [self checkState];
}
//- (void)xx_setEnabled:(BOOL)enabled{
//    [self xx_setEnabled:enabled];
//    [self checkState];
//}
//- (void)xx_setHighlighted:(BOOL)highlighted{
//    [self xx_setHighlighted:highlighted];
//    [self checkState];
//}
- (void)xx_setState:(UIControlState)state{
    [self xx_setState:state];
    [self checkState];
}

- (void) checkState{
    NSLog(@"[StateSettings] text:%@ state:%lu", self.titleLabel.text, (unsigned long)self.state);
//    NSMutableDictionary *allSettings = self.stateToSettings;
//    NSEnumerator *enumer = allSettings.keyEnumerator;
//    NSNumber *stateNumber = nil;
//    while (nil != (stateNumber = enumer.nextObject)) {
//        UIControlState state = [stateNumber integerValue];
//        if (state == UIControlStateNormal) {
//            if(UIControlStateHighlighted == self.state || state == self.state){
//                NSDictionary *settings = allSettings[stateNumber];
//                if(settings[kBorderColor]) self.layer.borderColor = [settings[kBorderColor] CGColor];
//                if(settings[kBackgroundColor]) self.backgroundColor = settings[kBackgroundColor];
//                break;
//            }
//        }
//        else{
//            if((self.state | state) == self.state){
//                NSDictionary *settings = allSettings[stateNumber];
//                if(settings[kBorderColor]) self.layer.borderColor = [settings[kBorderColor] CGColor];
//                if(settings[kBackgroundColor]) self.backgroundColor = settings[kBackgroundColor];
//                break;
//            }
//        }
//    }
}

@end
