//
//  UIButton+XX.m
//  Bolts
//
//  Created by VINSON on 2019/10/16.
//

#import "UIButton+StateSettings.h"
#import <objc/runtime.h>

const static char * const kXXstateToSettings    = "XXstateToSettings";
const static NSString * const kBorderColor      = @"XXborderColor";
const static NSString * const kBackgroundColor  = @"XXbackgroundColor";

@interface UIButton()
@property (nonatomic,strong) NSMutableDictionary *stateToSettings;
@end

@implementation UIButton(StateSettings)
- (void)setStateToSettings:(NSMutableDictionary *)stateToSettings{
    objc_setAssociatedObject(self, kXXstateToSettings, stateToSettings, OBJC_ASSOCIATION_RETAIN_NONATOMIC);
}
- (NSMutableDictionary *)stateToSettings{
    NSMutableDictionary *dict = objc_getAssociatedObject(self, kXXstateToSettings);
    if(!dict){
        dict = [NSMutableDictionary new];
        objc_setAssociatedObject(self, kXXstateToSettings, dict, OBJC_ASSOCIATION_RETAIN_NONATOMIC);
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


//- (void)setSelected:(BOOL)selected{
//    [super setSelected:selected];
//    [self checkState];
//}
- (void) checkState{
    NSMutableDictionary *allSettings = self.stateToSettings;
    NSEnumerator *enumer = allSettings.keyEnumerator;
    NSNumber *stateNumber = nil;
    while (nil != (stateNumber = enumer.nextObject)) {
        UIControlState state = [stateNumber integerValue];
        if (state == UIControlStateNormal) {
            if(UIControlStateHighlighted == self.state || state == self.state){
                NSDictionary *settings = allSettings[stateNumber];
                if(settings[kBorderColor]) self.layer.borderColor = [settings[kBorderColor] CGColor];
                if(settings[kBackgroundColor]) self.backgroundColor = settings[kBackgroundColor];
                break;
            }
        }
        else{
            if((self.state | state) == self.state){
                NSDictionary *settings = allSettings[stateNumber];
                if(settings[kBorderColor]) self.layer.borderColor = [settings[kBorderColor] CGColor];
                if(settings[kBackgroundColor]) self.backgroundColor = settings[kBackgroundColor];
                break;
            }
        }
    }
}

@end
