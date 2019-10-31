#import "UITableView+XXshell.h"
#import "NSString+JSON.h"
#import <objc/runtime.h>
#import "../Utility/XXbundleManager.h"

#import "XXheaderCellFooterBase.h"
#import "../Style/UILabel+XXstyle.h"
#import "../Style/UIView+XXstyle.h"
#import "../Utility/NSBundle+XXlanguage.h"

#define kShellGeneralKey    @"XXshellGeneral"
#define kShellContentKey    @"XXshellContent"
#define kShellStyleKey      @"XXshellStyle"
#define kCellID             @"XXcell"
#define kHeaderID           @"XXheader"
#define kFooterID           @"XXfooter"

#define kGeneral    @"General"
#define kBundle     @"Bundle"
#define kCellType   @"CellType"
#define kCellName   @"CellName"
#define kCellStyle  @"CellStyle"
#define kCellHeight @"CellHeight"

#define kHeaderType     @"HeaderType"
#define kHeaderName     @"HeaderName"
#define kHeaderHeight   @"HeaderHeight"

#define kFooterType     @"FooterType"
#define kFooterName     @"FooterName"
#define kFooterHeight   @"FooterHeight"

#define kContent    @"Content"
#define kHeader     @"Header"
#define kRow        @"Row"
#define kFooter     @"Footer"
#define kTitle      @"Title"
#define kDetail     @"Detail"
#define kImage      @"Image"
#define kAccessory  @"Accessory"

#define kDefault    @"Default"
#define kValue1     @"Value1"
#define kValue2     @"Value2"
#define kSubTitle   @"SubTitle"
#define kNib        @"Nib"
#define kCode       @"Code"
#define kStoryboard @"Storyboard"

#define kNone                   @"None"
#define kDisclosureIndicator    @"DisclosureIndicator"
#define kDisclosureButton       @"DisclosureButton"
#define kCheckMark              @"CheckMark"
#define kDetailButton           @"DetailButton"

#define kStyle  @"Style"

#define IS_CUSTOM_CELL(cell) (nil != cell && ([cell isEqualToString:kNib] || [cell isEqualToString:kCode] || [cell isEqualToString:kStoryboard]))
#define IS_CUSTOM_HEADER(header) (nil != header && ([header isEqualToString:kNib] || [header isEqualToString:kCode] || [header isEqualToString:kStoryboard]))
#define IS_CUSTOM_FOOTER(footer) (nil != footer && ([footer isEqualToString:kNib] || [footer isEqualToString:kCode] || [footer isEqualToString:kStoryboard]))

@interface UITableView()<UITableViewDataSource,UITableViewDelegate>
@property (nonatomic,strong) NSDictionary *shellGeneral;
@property (nonatomic,strong) NSMutableArray *shellContent;
@property (nonatomic,strong) NSDictionary *shellStyle;
@end

@implementation UITableView (XXshell)
#pragma mark - 公共接口: <全部配置> <内容设置> <样式设置> <行为触发>
- (void)generalWithJDictionary:(nonnull NSDictionary *)jdict {
    if(nil == self.delegate || nil == self.dataSource){
        self.delegate = self;
        self.dataSource = self;
    }
    if(nil != jdict[kGeneral]){
        self.shellGeneral = jdict[kGeneral];
        if(nil != jdict[kContent]){
            self.shellContent = [self contentMutable:jdict[kContent]];
        }
        if(nil != jdict[kStyle]){
            self.shellStyle = jdict[kStyle];
        }
    }
    else{
        self.shellGeneral = jdict;
    }
}
- (void)generalWithJString:(nonnull NSString *)jstring {
    NSDictionary *jdict = [jstring jsonObject];
    [self generalWithJDictionary:jdict];
}
- (void)contentWithtJString:(NSString *)jstring{
    NSArray *jarray = [jstring jsonObject];
    [self contentWithJArray:jarray];
}
- (void)contentWithJArray:(NSArray *)jarray{
    if(nil == jarray){
        self.shellContent = nil;
    }
    else{
        self.shellContent = [self contentMutable:jarray];
    }
}
- (void)actionWithJDictionary:(nonnull NSDictionary *)jdict {
}


#pragma mark - 属性读写: <General数据> <Content数据> <Style数据>
- (void)setShellGeneral:(NSDictionary *)shellGeneral{
    objc_setAssociatedObject(self, kShellGeneralKey, shellGeneral, OBJC_ASSOCIATION_RETAIN_NONATOMIC);
    [self generalTableView:shellGeneral];
}
- (NSDictionary*)shellGeneral{
    return objc_getAssociatedObject(self, kShellGeneralKey);
}
- (void)setShellContent:(NSMutableArray *)shellContent{
    objc_setAssociatedObject(self, kShellContentKey, shellContent, OBJC_ASSOCIATION_RETAIN_NONATOMIC);
}
- (NSMutableArray*)shellContent{
    return objc_getAssociatedObject(self, kShellContentKey);
}
- (void)setShellStyle:(NSDictionary *)shellStyle{
    objc_setAssociatedObject(self, kShellStyleKey, shellStyle, OBJC_ASSOCIATION_RETAIN_NONATOMIC);
    [self styleTableView:shellStyle];
}
- (NSDictionary *)shellStyle{
    return objc_getAssociatedObject(self, kShellStyleKey);
}

#pragma mark - 私有函数: <配置TableView> <Content可修改化>
- (void) generalTableView:(NSDictionary*)data{
    NSString *bundle    = data[kBundle];
    
    // cell
    NSString *cellType  = data[kCellType];
    NSString *cellName  = data[kCellName];
    if([cellType isEqualToString:kNib]){
        [self registerNib:[XXbundleManager nib:cellName inBundle:bundle] forCellReuseIdentifier:kCellID];
    }
    else if([cellType isEqualToString:kCode]){
        [self registerClass:NSClassFromString(cellName) forCellReuseIdentifier:kCellID];
    }
    else{
        
    }
    
    // header
    NSString *headerType = data[kHeaderType];
    NSString *headerName = data[kHeaderName];
    if([headerType isEqualToString:kNib]){
        [self registerNib:[XXbundleManager nib:headerName inBundle:bundle] forHeaderFooterViewReuseIdentifier:kHeaderID];
    }
    else if([headerType isEqualToString:kCode]){
        [self registerClass:NSClassFromString(headerName) forCellReuseIdentifier:kHeaderID];
    }
    else{
        
    }
    
    // footer
    NSString *footerType = data[kFooterType];
    NSString *footerName = data[kFooterName];
    if([footerType isEqualToString:kNib]){
        [self registerNib:[XXbundleManager nib:footerName inBundle:bundle] forHeaderFooterViewReuseIdentifier:kHeaderID];
    }
    else if([footerType isEqualToString:kCode]){
        [self registerClass:NSClassFromString(footerName) forCellReuseIdentifier:kFooterID];
    }
    else{
        
    }
    
    self.estimatedRowHeight     = nil == data[kCellHeight] ? 40 : [data[kCellHeight] floatValue];
}
-(void)styleTableView:(NSDictionary*)data{
    if(nil == data){
        return;
    }
    [self styleWithJDictionary:data];
}
- (NSMutableArray*) contentMutable:(NSArray*)content{
    NSMutableArray *array = [[NSMutableArray alloc] initWithCapacity:content.count];
    for (NSDictionary *section in content) {
        NSMutableDictionary *sectionMutable = [[NSMutableDictionary alloc] init];
        if(nil != [section objectForKey:kHeader]){
            sectionMutable[kHeader] = [[NSMutableDictionary alloc] initWithDictionary:section[kHeader]];
        }
        if(nil != [section objectForKey:kFooter]){
            sectionMutable[kFooter] = [[NSMutableDictionary alloc] initWithDictionary:section[kFooter]];
        }
        if(nil != [section objectForKey:kRow]){
            sectionMutable[kRow] = [[NSMutableArray alloc] initWithArray:section[kRow]];
        }
        [array addObject:sectionMutable];
    }
    return array;
}

#pragma mark - 私有函数: <CellStyle的值转换> <CellAccessaryType值转换> <更新原生Cell数据>
- (UITableViewCellStyle)toCellStyle:(NSString*)style{
    if([style isEqualToString:kDefault]){
        return UITableViewCellStyleDefault;
    }
    else if([style isEqualToString:kValue1]){
        return UITableViewCellStyleValue1;
    }
    else if([style isEqualToString:kValue2]){
        return UITableViewCellStyleValue2;
    }
    else if([style isEqualToString:kSubTitle]){
        return UITableViewCellStyleSubtitle;
    }
    else{
        return UITableViewCellStyleDefault;
    }
}
- (UITableViewCellAccessoryType)toCellAccessoryType:(NSString*)type{
    if([type isEqualToString:kNone]){
        return UITableViewCellAccessoryNone;
    }
    else if([type isEqualToString:kDisclosureIndicator]){
        return UITableViewCellAccessoryDisclosureIndicator;
    }
    else if([type isEqualToString:kDisclosureButton]){
        return UITableViewCellAccessoryDetailDisclosureButton;
    }
    else if([type isEqualToString:kCheckMark]){
        return UITableViewCellAccessoryCheckmark;
    }
    else if([type isEqualToString:kDetailButton]){
        return UITableViewCellAccessoryDetailButton;
    }
    else {
        return UITableViewCellAccessoryNone;
    }
}
- (void) updateDefalutCell:(UITableViewCell*)cell Data:(NSDictionary*)data Bundle:(NSString*)bundle{
    if(nil != data[kTitle])     cell.textLabel.text = data[kTitle];
    if(nil != data[kDetail])    cell.detailTextLabel.text = data[kDetail];
    if(nil != data[kImage])     cell.imageView.image = [XXbundleManager image:data[kImage] inBundle:bundle];
    if(nil != data[kAccessory]) cell.accessoryType = [self toCellAccessoryType:data[kAccessory]];
}

#pragma mark - 协议函数: <UITableViewDataSource>
- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView{
    return nil == self.shellContent ? 0 : self.shellContent.count;
}
- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section{
    return [self.shellContent[section][kRow] count];
}
- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath{
    /// [1]
    NSString *cellType = self.shellGeneral[kCellType];
    NSString *cellName = self.shellGeneral[kCellName];
    BOOL isUsingCustomCell = IS_CUSTOM_CELL(cellType);
    
    /// [2] 从复用池中取出cell
    UITableViewCell *cell = nil;
    if(isUsingCustomCell){
        cell = [tableView dequeueReusableCellWithIdentifier:kCellID forIndexPath:indexPath];
    }
    else {
        cell = [tableView dequeueReusableCellWithIdentifier:kCellID];
    }
    
    /// [3] 创建新的cell
    if(nil == cell){
        if(isUsingCustomCell)
            cell = [[NSClassFromString(cellName) alloc] initWithReuseIdentifier:kCellID];
        else
            cell = [[UITableViewCell alloc] initWithStyle:[self toCellStyle:self.shellGeneral[kCellStyle]] reuseIdentifier:kCellID];
    }
    
    /// [4] cell的数据设置
    NSDictionary *rowData = self.shellContent[indexPath.section][kRow][indexPath.row];
    if(isUsingCustomCell){
        XXheaderCellFooterBase *xxcell = (XXheaderCellFooterBase*)cell;
        xxcell.indexPath = indexPath;
        [xxcell setJData:rowData];
    }
    else{
        [self updateDefalutCell:cell Data:rowData Bundle:self.shellGeneral[kBundle]];
    }
    return cell;
}

#pragma mark - 协议函数(UITableViewDelegate): <点击> <页头控件> <页头高度> <页头即将显示> <页脚控件> <页脚高度> <页脚即将显示>
- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath{
    NSMutableDictionary *userInfo = [[NSMutableDictionary alloc] init];
    userInfo[@"IndexPath"] = indexPath;
    if(IS_CUSTOM_CELL(self.shellGeneral[kCellType])){
        NSDictionary *jdata = self.shellContent[indexPath.section][kRow][indexPath.row];
        if(nil != jdata){
            userInfo[@"JData"] = jdata;
        }
    }
    [[NSNotificationCenter defaultCenter] postNotificationName:TABLEVIEW_SIG_SELECTED object:nil userInfo:userInfo];
}
- (UIView *)tableView:(UITableView *)tableView viewForHeaderInSection:(NSInteger)section{
    if(nil == self.shellGeneral[kHeaderHeight]){
        return nil;
    }
        
    BOOL isUsingCustomHeader = IS_CUSTOM_HEADER(self.shellGeneral[kHeaderType]);
    UITableViewHeaderFooterView *header = [tableView dequeueReusableHeaderFooterViewWithIdentifier:kHeaderID];
    if(nil == header){
        if(isUsingCustomHeader){
            header = [[NSClassFromString(self.shellGeneral[kHeaderName]) alloc] initWithReuseIdentifier:kHeaderID];
        }
        else{
            header = [[UITableViewHeaderFooterView alloc] initWithReuseIdentifier:kHeaderID];
        }
    }
    
    NSDictionary *info = self.shellContent[section][kHeader];
    if(isUsingCustomHeader){
        XXheaderCellFooterBase *headerBase = (XXheaderCellFooterBase*)header;
        [headerBase setJData:info];
    }
    else{
        header.textLabel.text = XXTR(info[kTitle]);
    }
    return header;
}
- (void)tableView:(UITableView *)tableView willDisplayHeaderView:(UIView *)view forSection:(NSInteger)section{
    /// TODO - 使用默认的header配置字体颜色
    BOOL isUsingCustomHeader = IS_CUSTOM_HEADER(self.shellGeneral[kHeaderType]);
    if(isUsingCustomHeader){
        
    }
    else{
        if(nil != self.shellStyle[kHeader]){
            UITableViewHeaderFooterView *header = (UITableViewHeaderFooterView*)view;
            //[header styleWithJDictionary:self.shellStyle[kHeader]];
            [header.textLabel styleWithJDictionary:self.shellStyle[kHeader]];
        }
    }
}
- (CGFloat)tableView:(UITableView *)tableView heightForHeaderInSection:(NSInteger)section{
    return nil == self.shellGeneral[kHeaderHeight] ? 0.01 : [self.shellGeneral[kHeaderHeight] floatValue];
}
- (UIView *)tableView:(UITableView *)tableView viewForFooterInSection:(NSInteger)section{
    if(nil == self.shellGeneral[kFooterHeight]){
        return nil;
    }
    
    BOOL isUsingCustomFooter = IS_CUSTOM_HEADER(self.shellGeneral[kFooterType]);
    UITableViewHeaderFooterView *footer = [tableView dequeueReusableHeaderFooterViewWithIdentifier:kFooterID];
    if(nil == footer){
        if(isUsingCustomFooter){
            footer = [[NSClassFromString(self.shellGeneral[kFooterName]) alloc] initWithReuseIdentifier:kFooterID];
        }
        else{
            footer = [[UITableViewHeaderFooterView alloc] initWithReuseIdentifier:kFooterID];
        }
    }
    
    NSDictionary *info = self.shellContent[section][kFooter];
    if(isUsingCustomFooter){
        XXheaderCellFooterBase *footerBase = (XXheaderCellFooterBase*)footer;
        [footerBase setJData:info];
    }
    else{
        footer.textLabel.text   = info[kTitle];
    }
    return footer;
}
- (CGFloat)tableView:(UITableView *)tableView heightForFooterInSection:(NSInteger)section{
    return nil == self.shellGeneral[kFooterHeight] ? 0.01 : [self.shellGeneral[kFooterHeight] floatValue];
}
- (void)tableView:(UITableView *)tableView willDisplayFooterView:(UIView *)view forSection:(NSInteger)section{
    /// TODO - 使用默认的header配置字体颜色
}
@end

