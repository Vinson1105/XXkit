//
//  XXdebugHelper.m
//  XXkitProject
//
//  Created by VINSON on 2020/11/12.
//  Copyright © 2020 郭文轩. All rights reserved.
//

#import "XXdebugHelper.h"
#import "../XXocUtils.h"
#import "../Shell/XXtableViewShell.h"
#import "../Category/UIView+Touchable.m"
#import "../Category/UIView+Movable.h"


static XXdebugHelper *_instance = nil;
static NSString * const kSplitOfProperty = @"=";
static NSString * const kSplitOfEnum = @",";
static NSString * const kType = @"type";
static NSString * const kDefault = @"default";
static NSString * const kValue = @"value";

// MARK: 通用属性
typedef enum : NSUInteger {
    XXpropertyTypeInt,
    XXpropertyTypeFloat,
    XXpropertyTypeBool,
    XXpropertyTypeString,
    XXpropertyTypeSingleEnum,
    XXpropertyTypeMultiEnum,
} XXpropertyType;
@interface XXproperty : NSObject<NSCopying>
@property (nonatomic,copy,readonly) NSString *infoString;
@property (nonatomic,assign,readonly) XXpropertyType type;
@property (nonatomic,copy,readonly,nullable) id def;
@property (nonatomic,copy,nullable) id val;
+(nullable XXproperty*)propertyFromString:(NSString*)string;
@end
@implementation XXproperty
+(XXproperty*)propertyFromString:(NSString*)string{
    XXproperty *property = [XXproperty new];
    if(![property reset:string]){
        return nil;
    }
    return property;
}
-(instancetype)initWithProperty:(XXproperty*)property{
    self = [super init];
    if(self){
        _type = property.type;
        _def = property.def?[property.def copy]:nil;
        _val = property.val?[property.val copy]:nil;
    }
    return self;
}
-(BOOL)reset:(NSString*)string{
    NSArray *nodes = [string componentsSeparatedByString:kSplitOfProperty];
    if(nodes.count>3 || 0==nodes.count){
        return NO;
    }
    
    // 类型转换
    XXpropertyType type = -1;
    NSString *typeString = nodes[0];
    if([typeString isEqualToString:@"int"]){
        type = XXpropertyTypeInt;
    }
    else if([typeString isEqualToString:@"float"]){
        type = XXpropertyTypeFloat;
    }
    else if([typeString isEqualToString:@"bool"]){
        type = XXpropertyTypeBool;
    }
    else if([typeString isEqualToString:@"string"]){
        type = XXpropertyTypeString;
    }
    else if([typeString hasPrefix:@"["]){
        if([typeString hasSuffix:@"]"]){
            type = XXpropertyTypeSingleEnum;
        }
        else if([typeString hasSuffix:@"]*"]){
            type = XXpropertyTypeMultiEnum;
        }
        else{
            
        }
    }
    else{
        
    }
    if(-1==type){
        return NO;
    }
    _type = type;
    
    // 默认值
    _def = nil;
    if(nodes.count>=2){
        _def = [[self toValueFromString:nodes[1] forPropertyType:type] copy];
    }
    
    // 实际值
    _val = [self.def copy];
    
    return YES;
}
-(nullable id)toValueFromString:(NSString*)string forPropertyType:(XXpropertyType)propertyType{
    switch (propertyType) {
        case XXpropertyTypeInt:
            return @([string intValue]);
        case XXpropertyTypeFloat:
            return @([string floatValue]);
        case XXpropertyTypeBool:
            return @([string boolValue]);
        case XXpropertyTypeString:
            return string;
        case XXpropertyTypeSingleEnum:
            return string;
        case XXpropertyTypeMultiEnum:
            return [string componentsSeparatedByString:kSplitOfEnum];
        default:
            return nil;
    }
}
- (nonnull id)copyWithZone:(nullable NSZone *)zone {
    XXproperty *property = [[self.class allocWithZone:zone] initWithProperty:self];
    return property;
}
@end

// MARK: 枚举属性
@interface XXproperty(Enum)
@property (nonatomic,strong,readonly) NSArray *enum_availableItem;
@end
@implementation XXproperty(Enum)
+ (void)load{
    [XXocUtils replaceMethod:[self class] src:@selector(enum_reset:) dest:@selector(reset:)];
}
- (BOOL)enum_reset:(NSString*)string{
    
}
- (NSArray *)enum_availableItem{
    
}
@end

// MARK: 调试器数据模型
@interface DebugSettingModel : NSObject
@property (nonatomic,strong) NSMutableDictionary *nameToProperty;
@end
@implementation DebugSettingModel
- (instancetype)init{
    self = [super init];
    if (self) {
        self.nameToProperty = [NSMutableDictionary new];
    }
    return self;
}
-(void)configPropertyName:(NSString*)name type:(NSString*)type{
    XXproperty *property = [XXproperty propertyFromString:type];
    if(nil == property){
        return;
    }
    self.nameToProperty[name] = property;
}
-(nullable id)getPropertyValueWithName:(NSString*)name{
    if()
}
@end

// MARK: 调试器设置页面
@interface DebugSettingView : UIView
@property (nonatomic,weak) UIWindow *window;
@property (nonatomic,strong) UITableView *tableView;
@property (nonatomic,strong) UIButton *okButton;
@property (nonatomic,strong) UIButton *retButton;

@property (nonatomic,assign,readonly) BOOL popup;
@property (nonatomic,assign) CGRect popupFromRect;
@property (nonatomic,strong) XXtableViewShell *tableViewShell;
@end
@implementation DebugSettingView
- (instancetype)init{
    self = [super init];
    if (self) {
        _popup = NO;
        self.backgroundColor = UIColor.whiteColor;
        
        // cancel
        self.retButton = [UIButton buttonWithType:UIButtonTypeCustom];
        self.retButton.translatesAutoresizingMaskIntoConstraints = NO;
        [self.retButton setTitle:@"Cancel" forState:UIControlStateNormal];
        [self.retButton setTitleColor:UIColor.whiteColor forState:UIControlStateNormal];
        self.retButton.layer.cornerRadius = 22.5;
        self.retButton.backgroundColor = UIColor.redColor;
        [self addSubview:self.retButton];
        [self.retButton.leadingAnchor constraintEqualToAnchor:self.leadingAnchor constant:20].active = YES;
        [self.retButton.trailingAnchor constraintEqualToAnchor:self.trailingAnchor constant:-20].active = YES;
        [self.retButton.bottomAnchor constraintEqualToAnchor:self.bottomAnchor constant:-20].active = YES;
        NSLayoutConstraint *constraint = [self.retButton.heightAnchor constraintEqualToConstant:45];
        constraint.priority = 250;
        constraint.active = YES;
        [self.retButton addTarget:self action:@selector(onTouchUpInside:) forControlEvents:UIControlEventTouchUpInside];
        
        // ok
        self.okButton = [UIButton buttonWithType:UIButtonTypeCustom];
        self.okButton.translatesAutoresizingMaskIntoConstraints = NO;
        [self.okButton setTitle:@"OK" forState:UIControlStateNormal];
        [self.okButton setTitleColor:UIColor.whiteColor forState:UIControlStateNormal];
        self.okButton.layer.cornerRadius = 22.5;
        self.okButton.backgroundColor = UIColor.greenColor;
        [self addSubview:self.okButton];
        [self.okButton.leadingAnchor constraintEqualToAnchor:self.leadingAnchor constant:20].active = YES;
        [self.okButton.trailingAnchor constraintEqualToAnchor:self.trailingAnchor constant:-20].active = YES;
        [self.okButton.bottomAnchor constraintEqualToAnchor:self.retButton.topAnchor constant:-10].active = YES;
        constraint = [self.okButton.heightAnchor constraintEqualToConstant:45];
        constraint.priority = 250;
        constraint.active = YES;
        [self.okButton addTarget:self action:@selector(onTouchUpInside:) forControlEvents:UIControlEventTouchUpInside];
        
        // table
        self.tableView = [[UITableView alloc] initWithFrame:CGRectMake(0, 0, 0, 0) style:UITableViewStyleGrouped];
        self.tableView.translatesAutoresizingMaskIntoConstraints = NO;
        [self addSubview:self.tableView];
        [self.tableView.leadingAnchor constraintEqualToAnchor:self.leadingAnchor].active = YES;
        [self.tableView.trailingAnchor constraintEqualToAnchor:self.trailingAnchor].active = YES;
        [self.tableView.topAnchor constraintEqualToAnchor:self.topAnchor].active = YES;
        [self.tableView.bottomAnchor constraintEqualToAnchor:self.okButton.topAnchor constant:-20].active = YES;
        
        // table shell
        self.tableViewShell = [XXtableViewShell new];
        [self.tableViewShell shell:self.tableView];
        [self.tableViewShell configSectionRowSystemStyle:UITableViewCellStyleDefault height:40];
        [self.tableViewShell configSectionHeaderClass:nil loadType:-1 height:40];
        [self.tableViewShell configSectionFooterClass:nil loadType:-1 height:0.01];
    }
    return self;
}
-(void)popupFromRect:(CGRect)rect{
    if(self.popup) return;
    _popup = YES;
    
    self.popupFromRect = rect;
    self.frame = rect;
    self.alpha = 0;
    [self.window addSubview:self];
    [self.window bringSubviewToFront:self];
    
    CGRect endRect = self.window.bounds;
    [UIView animateWithDuration:0.2 animations:^{
        self.alpha = 1;
        self.frame = endRect;
    }];
}
-(void)popdown{
    if(!self.popup) return;
    _popup = NO;
    
    [UIView animateWithDuration:0.2 animations:^{
        self.alpha = 0;
        self.frame = self.popupFromRect;
        } completion:^(BOOL finished) {
            [self removeFromSuperview];
        }];
}
- (void)setWindow:(UIWindow *)window{
    if(window == _window) return;
    _window = window;
    if(self.popup){
        if(window){
            self.frame = window.bounds;
            [window addSubview:self];
            [window bringSubviewToFront:self];
        }
        else{
            _popup = NO;
            [self removeFromSuperview];
        }
    }
}
-(void)onTouchUpInside:(UIButton*)button{
    if(button == self.okButton){
        
    }
    else if(button == self.retButton){
        [self popdown];
    }
    else {
        
    }
}
@end

// MARK: 调试器主控
@interface XXdebugHelper()
@property (nonatomic,weak) UIWindow *window;
@property (nonatomic,strong) UIView *indicaterView;
@property (nonatomic,strong) DebugSettingView *settingView;
@property (nonatomic,strong) NSMutableDictionary *nameToType;
@end

@implementation XXdebugHelper
+(XXdebugHelper*)sharedInstance{
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        _instance = [XXdebugHelper new];
    });
    return _instance;
}
+(void)install:(UIWindow*)window{
    [XXdebugHelper sharedInstance].window = window;
}
+(void)configPropertyName:(NSString*)name type:(NSString*)type{
    [[XXdebugHelper sharedInstance] configPropertyName:name type:type];
}
+(nullable id)getPropertyWithName:(NSString*)name{
    return [[XXdebugHelper sharedInstance] getPropertyWithName:name];
}

- (instancetype)init{
    self = [super init];
    if (self) {
        XXOC_WS;
        
        // Indicater小组件
        self.indicaterView = [[UIView alloc] initWithFrame:CGRectMake(20, 80, 50, 50)];
        self.indicaterView.translatesAutoresizingMaskIntoConstraints = NO;
        self.indicaterView.backgroundColor = [UIColor colorWithRed:0.3 green:0.3 blue:0.3 alpha:0.5];
        self.indicaterView.layer.cornerRadius = 5;
        [self.indicaterView touchable_install:^{
            XXOC_SS;
            [ss onIndicaterViewTouched];
        }];
        [self.indicaterView movable_install:20];        
        
        // SettingView
        self.settingView = [DebugSettingView new];
        
        // name to type
        self.nameToType = [NSMutableDictionary new];
        [self configPropertyName:@"LogServer" type:@"string=192.168.0.1:32323"];
        [self configPropertyName:@"LogServerEnabled" type:@"bool=0"];
    }
    return self;
}
- (void)setWindow:(UIWindow *)window{
    if(window == _window) return;
    _window = window;
    if(_window){
        [_window addSubview:self.indicaterView];
    }
    else{
        [self.indicaterView removeFromSuperview];
    }
    self.settingView.window = window;
}
-(void)configPropertyName:(NSString*)name type:(NSString*)type{
    self.nameToType[name] = type;
}
-(nullable id)getPropertyWithName:(NSString*)name{
    return nil;
}
- (void)onIndicaterViewTouched{
    [self.settingView popupFromRect:self.indicaterView.frame];
}
@end
