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

#import "../Shell/XXviewBase.h"
#import "../Category/UIView+Popup.h"
#import "../Category/UIView+ModalPopup.h"
#import "../Category/UIView+KeyboradAdapter.h"

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
-(NSString*)stringFromPropertyType:(XXpropertyType)propertyType{
    switch (propertyType) {
        case XXpropertyTypeInt:
            return @"int";
        case XXpropertyTypeFloat:
            return @"float";
        case XXpropertyTypeBool:
            return @"bool";
        case XXpropertyTypeString:
            return @"string";
        case XXpropertyTypeSingleEnum:
            return @"singleEnum";
        case XXpropertyTypeMultiEnum:
            return @"multiEnum";
        default:
            return nil;
    }
}
- (BOOL)isEqualProperty:(XXproperty*)property{
    if(self.type != property.type){
        return NO;
    }
    if(![self.def isEqual:property.def]){
        return NO;
    }
    if(self.val && property.val && ![self.val isEqual:property.val]){
        return NO;
    }
    return YES;
}
- (NSString *)description{
    return [NSString stringWithFormat:@"[Property] def:%@ val:%@ type:%@", self.def, self.val, [self stringFromPropertyType:self.type]];
}
@end

// MARK: 枚举属性
@interface XXproperty(Enum)
@property (nonatomic,strong,readonly) NSArray *enum_availableItem;
@end
@implementation XXproperty(Enum)
+ (void)load{
    [XXocUtils replaceMethod:[self class] src:@selector(enum_reset:) dest:@selector(reset:)];
    [XXocUtils replaceMethod:[self class] src:@selector(enum_initWithProperty:) dest:@selector(initWithProperty:)];
}
- (BOOL)enum_reset:(NSString*)string{
    BOOL succeed = [self enum_reset:string];
    if(succeed && (XXpropertyTypeMultiEnum == self.type || XXpropertyTypeSingleEnum == self.type)){
        NSString *typeString            = [string componentsSeparatedByString:kSplitOfProperty].firstObject;
        NSRange range                   = NSMakeRange(1, typeString.length-XXpropertyTypeMultiEnum == self.type?3:2);
        NSString *availableItemString   = [typeString substringWithRange:range];
        NSArray *availableItems         = [availableItemString componentsSeparatedByString:kSplitOfEnum];
        objc_setAssociatedObject(self, "enum_availableItem", availableItems, OBJC_ASSOCIATION_COPY_NONATOMIC);
    }
    return succeed;
}
- (NSArray *)enum_availableItem{
    return objc_getAssociatedObject(self, "enum_availableItem");
}
-(instancetype)enum_initWithProperty:(XXproperty*)property{
    id obj = [self enum_initWithProperty:property];
    if(obj){
        NSArray *availableItem = [property.enum_availableItem copy];
        objc_setAssociatedObject(self, "enum_availableItem", availableItem, OBJC_ASSOCIATION_COPY_NONATOMIC);
    }
    return obj;
}
@end

// MARK: 属性编辑器
@protocol XXpropertyEditItemDelegate
@property (nonatomic,copy) XXproperty *property;
-(instancetype)initWithProperty:(XXproperty*)property;
-(UIEdgeInsets)estimatedMargin;
-(CGSize)estimatedSize;
-(void)finish;
-(void)addAction:(SEL)action forPropertyChangedAtTarget:(id)target;
@end

// MARK: bool类型属性编辑器Item
@interface BoolPropertyEditItem : UISwitch<XXpropertyEditItemDelegate>
@property (nonatomic,copy) XXproperty *property;
@property (nonatomic,weak) id target;
@property (nonatomic,assign) SEL action;
@end
@implementation BoolPropertyEditItem
- (instancetype)initWithProperty:(XXproperty *)property{
    self = [super init];
    if(self){
        self.property = property;
        [self addTarget:self action:@selector(onValueChanged) forControlEvents:UIControlEventValueChanged];
    }
    return self;
}
-(void)onValueChanged{
    self.property.val = @(self.on);
    if(self.target && self.action){
        void(*method)(id,SEL,XXproperty*) = (void*)[self.target methodForSelector:self.action];
        method(self.target, self.action, self.property);
    }
}
- (void)setProperty:(XXproperty *)property{
    if(nil==property){
        
    }
    else{
        _property = [property copy];
    }
    
    if(_property.val){
        self.on = [_property.val boolValue];
    }
    else if(_property.def){
        self.on = [_property.def boolValue];
    }
    else{
        self.on = NO;
    }
}
- (UIEdgeInsets)estimatedMargin{
    return UIEdgeInsetsMake(10, 10, -10, -10);
}
- (CGSize)estimatedSize{
    return CGSizeMake(-1, 0);
}
- (void)finish{
    
}
- (void)addAction:(SEL)action forPropertyChangedAtTarget:(id)target{
    self.action = action;
    self.target = target;
}
@end

// MARK: String类型属性编辑Item
@interface StringPropertyEditItem : UITextField<XXpropertyEditItemDelegate,UITextFieldDelegate>
@property (nonatomic,copy) XXproperty *property;
@property (nonatomic,weak) id target;
@property (nonatomic,assign) SEL action;
@end
@implementation StringPropertyEditItem
-(instancetype)initWithProperty:(XXproperty*)property{
    self = [super init];
    if (self) {
        self.delegate = self;
        self.property = property;
        self.borderStyle = UITextBorderStyleRoundedRect;
        self.returnKeyType = UIReturnKeyDone;
    }
    return self;
}
- (void)setProperty:(XXproperty *)property{
    if(nil == property){
        _property = nil;
    }
    else{
        _property = [property copy];
    }
    
    if(_property.val){
        self.text = [NSString stringWithFormat:@"%@",property.val];
    }
    else if(_property.def){
        self.text = [NSString stringWithFormat:@"%@",property.def];
    }
    else {
        self.text = @"";
    }
}
-(UIEdgeInsets)estimatedMargin{
    return UIEdgeInsetsMake(10, 10, -10, -10);
}
-(CGSize)estimatedSize{
    return CGSizeMake(0, 40);
}
- (void)finish{
    [self finishWithString:self.text];
}
-(void)addAction:(SEL)action forPropertyChangedAtTarget:(id)target{
    self.target = target;
    self.action = action;
}
- (void)textFieldDidEndEditing:(UITextField *)textField{
    [self finishWithString:textField.text];
}
- (BOOL)textFieldShouldReturn:(UITextField *)textField{
    [textField resignFirstResponder];
    return YES;
}
-(void)finishWithString:(NSString*)string{
    if(XXpropertyTypeString == self.property.type){
        if((self.property.val && ![string isEqualToString:self.property.val]) || ![string isEqualToString:self.property.def]){
            self.property.val = string;
        }
        else{
            return;
        }
    }
    else if(XXpropertyTypeInt == self.property.type){
        NSNumber *intNumber = @([string intValue]);
        if((self.property.val && ![intNumber isEqual:self.property.val]) || ![intNumber isEqual:self.property.def]){
            self.property.val = intNumber;
        }
        else{
            return;
        }
    }
    else if(XXpropertyTypeFloat == self.property.type){
        NSNumber *floatNumber = [NSDecimalNumber decimalNumberWithString:string];//@([string floatValue]);
        if((self.property.val && ![floatNumber isEqual:self.property.val]) || ![floatNumber isEqual:self.property.def]){
            self.property.val = floatNumber;
        }
        else{
            return;
        }
    }
    else {
        return;
    }

    if(!self.target || !self.action){
        return;
    }
    IMP method = [self.target methodForSelector:self.action];
    void (*methodPtr)(id, SEL, XXproperty*) = (void*)method;
    methodPtr(self.target,self.action,self.property);
}
@end


@interface XXpropertyEditView : UIView
@property (nonatomic,strong) UIView *hSplitView;
@property (nonatomic,strong) UIView *vSplitView;
@property (nonatomic,strong) UIButton *okButton;
@property (nonatomic,strong) UIButton *cancelButton;
@property (nonatomic,strong) NSMutableDictionary *typeToItem;
@property (nonatomic,strong) XXproperty *propertyBeforeEdit;
@property (nonatomic,strong) XXproperty *currentPropertyEditing;
@property (nonatomic,strong) UIView<XXpropertyEditItemDelegate> *currentPropertyItem;
@property (nonatomic,copy) void(^onFinishHandler)(XXproperty *_Nullable property, BOOL changed);
@end
static XXpropertyEditView *_editViewInstance = nil;
@implementation XXpropertyEditView
+(XXpropertyEditView*)sharedInstance{
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        _editViewInstance = [XXpropertyEditView new];
    });
    return _editViewInstance;
}
+(void)popupWithProperty:(XXproperty*)property finishHandler:(void(^)(id value, BOOL changed))finishhandler{
    [[XXpropertyEditView sharedInstance] popupWithProperty:property finishHandler:finishhandler];
}
+(void)popdown{
    [[XXpropertyEditView sharedInstance] popdown];
}
- (instancetype)init{
    self = [super init];
    if (self) {
        self.okButton = [UIButton buttonWithType:UIButtonTypeCustom];
        self.okButton.translatesAutoresizingMaskIntoConstraints = NO;
        self.okButton.titleLabel.font = [UIFont systemFontOfSize:15];
        [self.okButton setTitle:@"好的" forState:UIControlStateNormal];
        [self.okButton setTitleColor:UIColor.systemBlueColor forState:UIControlStateNormal];
        [self.okButton addTarget:self action:@selector(onTouchUpInside:) forControlEvents:UIControlEventTouchUpInside];
        [self addSubview:self.okButton];
        
        self.cancelButton = [UIButton buttonWithType:UIButtonTypeCustom];
        self.cancelButton.translatesAutoresizingMaskIntoConstraints = NO;
        self.cancelButton.titleLabel.font = [UIFont systemFontOfSize:15];
        [self.cancelButton setTitle:@"取消" forState:UIControlStateNormal];
        [self.cancelButton setTitleColor:UIColor.systemRedColor forState:UIControlStateNormal];
        [self.cancelButton addTarget:self action:@selector(onTouchUpInside:) forControlEvents:UIControlEventTouchUpInside];
        [self addSubview:self.cancelButton];
        
        self.hSplitView = [UIView new];
        self.hSplitView.translatesAutoresizingMaskIntoConstraints = NO;
        self.hSplitView.backgroundColor = [XXocUtils colorFromLightHex:@"#cfcfcf" darkHex:@"#1f1f1f"];
        [self addSubview:self.hSplitView];
        
        self.vSplitView = [UIView new];
        self.vSplitView.translatesAutoresizingMaskIntoConstraints = NO;
        self.vSplitView.backgroundColor = [XXocUtils colorFromLightHex:@"#cfcfcf" darkHex:@"#1f1f1f"];
        [self addSubview:self.vSplitView];
        
        [self.hSplitView.heightAnchor constraintEqualToConstant:0.7].active = YES;
        [self.hSplitView.leadingAnchor constraintEqualToAnchor:self.leadingAnchor].active = YES;
        [self.hSplitView.trailingAnchor constraintEqualToAnchor:self.trailingAnchor].active = YES;
        [self.hSplitView.bottomAnchor constraintEqualToAnchor:self.bottomAnchor constant:-45].active = YES;
        
        [self.vSplitView.widthAnchor constraintEqualToConstant:0.7].active = YES;
        [self.vSplitView.centerXAnchor constraintEqualToAnchor:self.centerXAnchor].active = YES;
        [self.vSplitView.topAnchor constraintEqualToAnchor:self.hSplitView.bottomAnchor].active = YES;
        [self.vSplitView.bottomAnchor constraintEqualToAnchor:self.bottomAnchor].active = YES;
        
        [self.okButton.leadingAnchor constraintEqualToAnchor:self.leadingAnchor].active = YES;
        [self.okButton.topAnchor constraintEqualToAnchor:self.hSplitView.bottomAnchor].active = YES;
        [self.okButton.trailingAnchor constraintEqualToAnchor:self.vSplitView.leadingAnchor].active = YES;
        [self.okButton.bottomAnchor constraintEqualToAnchor:self.bottomAnchor].active = YES;
        
        [self.cancelButton.leadingAnchor constraintEqualToAnchor:self.vSplitView.trailingAnchor].active = YES;
        [self.cancelButton.topAnchor constraintEqualToAnchor:self.hSplitView.bottomAnchor].active = YES;
        [self.cancelButton.trailingAnchor constraintEqualToAnchor:self.trailingAnchor].active = YES;
        [self.cancelButton.bottomAnchor constraintEqualToAnchor:self.bottomAnchor].active = YES;
        
        self.modalPopup_backgroundColorTransparent = NO;
        self.modalPopup_touchBackgroundToPopdown = YES;
//        [self modalPopup_configAtBottomSize:CGSizeMake(0, 0) margin:20];
        
        NSMutableArray *upLayouts = [NSMutableArray new];
        [upLayouts addObject:[self.leadingAnchor constraintEqualToAnchor:self.modalPopup_backgroundView.leadingAnchor constant:20]];
        [upLayouts addObject:[self.trailingAnchor constraintEqualToAnchor:self.modalPopup_backgroundView.trailingAnchor constant:-20]];
        [upLayouts addObject:[self.bottomAnchor constraintEqualToAnchor:self.modalPopup_backgroundView.bottomAnchor constant:-20]];
        
        NSMutableArray *downLayouts = [NSMutableArray new];
        [downLayouts addObject:[self.leadingAnchor constraintEqualToAnchor:self.modalPopup_backgroundView.leadingAnchor constant:20]];
        [downLayouts addObject:[self.trailingAnchor constraintEqualToAnchor:self.modalPopup_backgroundView.trailingAnchor constant:-20]];
        [downLayouts addObject:[self.topAnchor constraintEqualToAnchor:self.modalPopup_backgroundView.bottomAnchor constant:20]];
        
        [self modalPopup_configPopupConstraint:upLayouts popdownConstraint:downLayouts];
        [self keyboradAdapter_install:YES];
        self.keyboradAdapter_vlayout = upLayouts[2];

        self.backgroundColor = [XXocUtils colorFromLightHex:@"ffffff" darkHex:@"#1f1f1f"];
        self.layer.cornerRadius = 20;
    }
    return self;
}
-(void)popupWithProperty:(XXproperty*)property finishHandler:(void(^)(id value, BOOL changed))finishhandler{
    UIView<XXpropertyEditItemDelegate> *editItem = self.typeToItem[@(property.type)];
    if(editItem){
        editItem.property = property;
    }
    else{
        editItem = [self createEditItemForProperty:property];
        if(editItem){
            editItem.translatesAutoresizingMaskIntoConstraints = NO;
            self.typeToItem[@(property.type)] = editItem;
            [editItem addAction:@selector(onPropertyChanged:) forPropertyChangedAtTarget:self];
        }
    }
    if(nil == editItem){
        return;
    }
    [self addSubview:editItem];
    
    
    CGSize size = [editItem estimatedSize];
    if(size.width>0){
        [editItem.widthAnchor constraintEqualToConstant:size.width].active = YES;;
    }
    if(size.height>0){
        [editItem.heightAnchor constraintEqualToConstant:size.height].active = YES;;
    }
    
    UIEdgeInsets margin = [editItem estimatedMargin];
    if(size.width>0||-1==size.width){
        [editItem.centerXAnchor constraintEqualToAnchor:self.centerXAnchor].active = YES;
    }
    else{
        [editItem.leadingAnchor constraintEqualToAnchor:self.leadingAnchor constant:margin.left].active = YES;
        [editItem.trailingAnchor constraintEqualToAnchor:self.trailingAnchor constant:margin.right].active = YES;
    }
    [editItem.topAnchor constraintEqualToAnchor:self.topAnchor constant:margin.top].active = YES;
    [editItem.bottomAnchor constraintEqualToAnchor:self.hSplitView.topAnchor constant:margin.bottom].active = YES;

    self.currentPropertyEditing = editItem.property;
    self.propertyBeforeEdit = property;
    self.currentPropertyItem = editItem;
    
    XXOC_WS;
    self.modalPopup_blockWhenDidDownFinished = ^{
        XXOC_SS;
        ss.currentPropertyEditing = nil;
        [editItem removeFromSuperview];
    };
    self.modalPopup_popup = YES;
    self.onFinishHandler = finishhandler;
}
-(void)popdown{
    self.modalPopup_popup = NO;
}
-(void)onTouchUpInside:(UIButton*)button{
    [self.currentPropertyItem finish];
    if(button == self.okButton){
        NSLog(@"[###] [onTouchUpInside] [ok] %@", self.currentPropertyEditing);
        if(self.onFinishHandler){
            self.onFinishHandler(self.currentPropertyEditing,
                                 ![self.currentPropertyEditing isEqualProperty:self.propertyBeforeEdit]);
        }
    }
    else if(button == self.cancelButton){
        NSLog(@"[###] [onTouchUpInside] [cancel] %@", self.currentPropertyEditing);
//        if(self.onFinishHandler){
//            self.onFinishHandler(nil, NO);
//        }
    }
    else{
        
    }
    [self popdown];
}
-(nullable UIView<XXpropertyEditItemDelegate>*)createEditItemForProperty:(XXproperty*)property{
    switch (property.type) {
        case XXpropertyTypeString:
        case XXpropertyTypeInt:
        case XXpropertyTypeFloat:
            return [[StringPropertyEditItem alloc] initWithProperty:property];
        case XXpropertyTypeBool:
            return [[BoolPropertyEditItem alloc] initWithProperty:property];
        default:
            return nil;
    }
}
-(void)onPropertyChanged:(XXproperty*)property{
    NSLog(@"[###] [onPropertyChanged] %@", property);
}
@end

// MARK: 调试器数据模型
@interface DebugSettingModel : NSObject
@property (nonatomic,strong) NSMutableDictionary *branchToPropertys;
@end
@implementation DebugSettingModel
- (instancetype)init{
    self = [super init];
    if (self) {
        self.branchToPropertys = [NSMutableDictionary new];
    }
    return self;
}
-(BOOL)addPropertyName:(NSString*)name type:(NSString*)type atBranch:(NSString*)branch{
    XXproperty *property = [XXproperty propertyFromString:type];
    if(nil == property){
        return NO;
    }
    
    NSMutableDictionary *propertys = self.branchToPropertys[branch];
    if(nil==propertys){
        propertys = [NSMutableDictionary new];
        self.branchToPropertys[branch] = propertys;
    }
    
    propertys[name] = property;
    return YES;
}
-(nullable XXproperty*)getPropertyWithName:(NSString*)name atBranch:(NSString*)branch{
    NSMutableDictionary *propertys = self.branchToPropertys[branch];
    if(nil==propertys){
        return nil;
    }
    return propertys[name];
}
-(nullable id)getPropertyValueWithName:(NSString*)name atBranch:(NSString*)branch{
    NSMutableDictionary *propertys = self.branchToPropertys[branch];
    if(nil==propertys){
        return nil;
    }
    XXproperty *property = propertys[name];
    if(nil==property){
        return nil;
    }
    return property.val;
}
-(nullable XXproperty*)updatePropertyValue:(id)value forName:(NSString*)name branch:(NSString*)branch{
    NSMutableDictionary *propertys = self.branchToPropertys[branch];
    if(nil==propertys){
        return nil;
    }
    XXproperty *property = propertys[name];
    if(nil==property){
        return nil;
    }
    property.val = value;
    return property;
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
@property (nonatomic,strong) DebugSettingModel *settingModel;
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
        [self.tableViewShell configSectionRowSystemStyle:UITableViewCellStyleValue1 height:50];
        [self.tableViewShell configSectionHeaderClass:nil loadType:-1 height:40];
        [self.tableViewShell configSectionFooterClass:nil loadType:-1 height:0.01];
        
        XXOC_WS;
        self.tableViewShell.onSectionRowClicked = ^(XXtableViewShell * _Nonnull shell,
                                                    NSIndexPath * _Nonnull indexPath,
                                                    id  _Nonnull data) {
            XXproperty *property = data[@"Property"];
            if(nil == property){
                return;
            }
            
            [[XXpropertyEditView sharedInstance] popupWithProperty:property finishHandler:nil];
            [XXpropertyEditView sharedInstance].onFinishHandler = ^(XXproperty * _Nullable property, BOOL changed) {
                if(!changed){
                    return;
                }
                XXOC_SS;
                NSString *path = data[@"Path"];
                NSRange range = [path rangeOfString:@"/"];
                if(range.location == NSNotFound){
                    return;
                }
                NSString *branch = [path substringWithRange:NSMakeRange(0, range.location)];
                NSString *name = [path substringWithRange:NSMakeRange(range.location+1, path.length-range.location-1)];
                [ss updateProperty:property forName:name branch:branch];
            };
        };
        
        // model
        self.settingModel = [DebugSettingModel new];
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
-(void)setWindow:(UIWindow *)window{
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
-(BOOL)addPropertyName:(NSString*)name type:(NSString*)type title:(NSString*)title atBranch:(NSString*)branch{
    BOOL succeed = [self.settingModel addPropertyName:name type:type atBranch:branch];
    if(!succeed){
        return NO;
    }
    XXproperty *property = [self.settingModel getPropertyWithName:name atBranch:branch];
    NSDictionary *row = @{
        @"Title":title,
        @"Detail":[NSString stringWithFormat:@"%@",property.val],
        @"Property":property,
        @"Path":[NSString stringWithFormat:@"%@/%@",branch,name]
    };
    
    NSInteger sectionIndex = [self.tableViewShell getSectionIndexWithHeaderKey:nil equelTo:branch];
    if(sectionIndex<0){
        [self.tableViewShell addSectionHeader:branch row:@[row] footer:nil];
    }
    else{
        [self.tableViewShell addSectionRows:@[row] atIndex:sectionIndex];
    }
    return YES;
}
-(nullable id)getPropertyValueWithName:(NSString*)name atBranch:(NSString *)branch{
    return [self.settingModel getPropertyValueWithName:name atBranch:branch];
}
-(void)updateProperty:(XXproperty*)property forName:(NSString*)name branch:(NSString*)branch{
    property = [self.settingModel updatePropertyValue:property.val forName:name branch:branch];
    [self.tableViewShell updateSectionRow:@{@"Detail":[NSString stringWithFormat:@"%@",property.val],@"Property":property}
                                      key:@"Path"
                                  equelTo:[NSString stringWithFormat:@"%@/%@",branch,name]];
}
@end

// MARK: 调试器主控
@interface XXdebugHelper()
@property (nonatomic,weak) UIWindow *window;
@property (nonatomic,strong) UIView *indicaterView;
@property (nonatomic,strong) DebugSettingView *settingView;
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
+(void)addPropertyName:(NSString*)name type:(NSString*)type title:(nullable NSString*)title atBranch:(nullable NSString*)branch{
    [[XXdebugHelper sharedInstance] addPropertyName:name type:type title:title atBranch:branch];
}
+(nullable id)getPropertyWithName:(NSString*)name atBranch:(nullable NSString *)branch{
    return [[XXdebugHelper sharedInstance] getPropertyWithName:name atBranch:branch];
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
        [self addPropertyName:@"LogServer" type:@"string=192.168.0.1:32323" title:@"显示终端IP" atBranch:@"Network"];
        [self addPropertyName:@"LogServerEnabled" type:@"bool=0" title:@"显示终端开关" atBranch:@"Network"];
        [self addPropertyName:@"int" type:@"int=111" title:@"整型" atBranch:@"branch1"];
        [self addPropertyName:@"float" type:@"float=222.222" title:@"浮点型" atBranch:@"branch2"];
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
-(void)addPropertyName:(NSString*)name type:(NSString*)type title:(NSString*)title atBranch:(NSString *)branch{
    if(![self.settingView addPropertyName:name type:type title:title atBranch:branch]){
        NSLog(@"[%@] 配置属性失败。（name:%@ type:%@）", NSStringFromClass(self.class), name, type);
        return;
    }
}
-(nullable id)getPropertyWithName:(NSString*)name atBranch:(NSString *)branch{
    return [self.settingView getPropertyValueWithName:name atBranch:branch];
}
- (void)onIndicaterViewTouched{
    [self.settingView popupFromRect:self.indicaterView.frame];
}
@end
