/**
 关于stateToSettings
 state:{
    BorderColor
    BackgroundColor
 }
 */

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
        [XXocUtils replaceMethod:[self class] src:@selector(setSelected:) dest:@selector(xx_setSelected:)];
        [XXocUtils replaceMethod:[self class] src:@selector(setHighlighted:) dest:@selector(xx_setHighlighted:)];
        [XXocUtils replaceMethod:[self class] src:@selector(setEnabled:) dest:@selector(xx_setEnabled:)];
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
    [XXocUtils mainThreadProcess:^{
        NSMutableDictionary *settings = self.stateToSettings[@(state)];
        if(nil == settings){
            settings = [NSMutableDictionary new];
            self.stateToSettings[@(state)] = settings;
        }
        settings[kBorderColor] = color;
        if(self.state == state){
            self.layer.borderColor = color.CGColor;
        }
    }];
}
- (void)setBackgroundColor:(UIColor *)color forState:(UIControlState)state{
    [XXocUtils mainThreadProcess:^{
        NSMutableDictionary *settings = self.stateToSettings[@(state)];
        if(nil == settings){
            settings = [NSMutableDictionary new];
            self.stateToSettings[@(state)] = settings;
        }
        settings[kBackgroundColor] = color;
        if(self.state == state){
            self.backgroundColor = color;
        }
    }];
}

- (void)xx_setSelected:(BOOL)selected{
    [self xx_setSelected:selected];
    [self checkState];
}
- (void)xx_setEnabled:(BOOL)enabled{
    [self xx_setEnabled:enabled];
    [self checkState];
}
- (void)xx_setHighlighted:(BOOL)highlighted{
    [self xx_setHighlighted:highlighted];
    [self checkState];
}

- (void) checkState{    
    NSMutableDictionary *settings = self.stateToSettings[@(self.state)];
    if(nil == settings){
        return;
    }
    
    UIColor *backgroundColor = settings[kBackgroundColor];
    if(nil != backgroundColor){
        [self setBackgroundColor:backgroundColor];
    }
    
    UIColor *borderColor = settings[kBorderColor];
    if(nil != borderColor){
        self.layer.borderColor = borderColor.CGColor;
    }
}

@end
