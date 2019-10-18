#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN

/**
 # 数据结构
 {
    Bundle:         // 省缺使用mainBundle
    CellType:       // Default/Value1/Value2/SubTitle/Nib/Code/Storyboard,省缺使用默认cell(Default)
    CellName:       // 当Cell指定为Nib或者Code时,不可省缺
    HeaderType:     // Default/Nib/Code/Storyboard,省缺使用默认Header
    HeaderName:     // 当Header指定为Nib或者Code时,不可省缺
    FooterType:     // Default/Nib/Code/Storyboard,省缺使用默认Footer
    FooterName:     // 当Footer指定为Nib或者Code时,不可省缺
    Content:        // 可省缺
 }
 
 ## Data
 Data:[
    {
        Header:     // 省缺时无Header显示
        Row:        // 可省缺
        Footer:     // 省缺时无Footer显示
    },
    {...},{...}
 ]
 
 ## Header/Footer
 Header/Footer:{
    Title:
    Detail:
 }
 
 ## Row
 Row:[
    {
        // 默认Cell
        Image:
        Title:
        Detail:
        Accessory:
    },{...},{...}
 ]
 */

#define TABLEVIEW_SHELL_BUNDLE      @"Bundle"
#define TABLEVIEW_SHELL_CELLTYPE    @"CellType"
#define TABLEVIEW_SHELL_CELLNAME    @"CellName"

#define TABLEVIEW_SHELL_HEADERTYPE  @"HeaderType"
#define TABLEVIEW_SHELL_HEADERNAME  @"HeaderName"
#define TABLEVIEW_SHELL_FOOTERTYPE  @"FooterType"
#define TABLEVIEW_SHELL_FOOTERNAME  @"FooterName"
#define TABLEVIEW_SHELL_CONTENT     @"Content"

#define TABLEVIEW_SHELL_HEADER      @"Header"
#define TABLEVIEW_SHELL_ROW         @"Row"
#define TABLEVIEW_SHELL_FOOTER      @"Footer"
#define TABLEVIEW_SHELL_TITLE       @"Title"
#define TABLEVIEW_SHELL_Detail      @"Detail"
#define TABLEVIEW_SHELL_IMAGE       @"Image"
#define TABLEVIEW_SHELL_ACCESSORY   @"Accessory"

#define TABLEVIEW_SHELL_DEFAULTE    @"Default"
#define TABLEVIEW_SHELL_VALUE1      @"Value1"
#define TABLEVIEW_SHELL_VALUE2      @"Value2"
#define TABLEVIEW_SHELL_SUBTITLE    @"SubTitle"
#define TABLEVIEW_SHELL_NIB         @"Nib"
#define TABLEVIEW_SHELL_CODE        @"Code"
#define TABLEVIEW_SHELL_STORYBOARD  @"Storyboard"

@interface UITableView (Shell)
-(void)installShellWithString:(NSString*)json;
-(void)installShellWithDictionary:(NSDictionary*)dict;
@end

NS_ASSUME_NONNULL_END
