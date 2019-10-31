/**
基本的table配置
Bundle:         // 资源对应的bundle,省缺使用默认的bundle
TableStyle:     // tableView的样式,Plain,Grouped,省缺使用Grouped

CellType:       // Default/Nib/Code/Storyboard,省缺使用默认Cell(Default)
CellName:       // 当Cell指定为Nib或者Code时,不可省缺
CellStyle:      // Default/Value1/Value2/SubTitle,使用默认Cell时有效,省缺时,使用默认样式(Default)
CellHeight:     // 全局Cell的高度,-1时为自适应,省缺时默认45px

HeaderType:     // Default/Nib/Code/Storyboard,省缺使用默认Header
HeaderName:     // 当Header指定为Nib或者Code时,不可省缺
HeaderHeight:   // 全局Header的高度,省缺时默认0.01

FooterType:     // Default/Nib/Code/Storyboard,省缺使用默认Footer
FooterName:     // 当Footer指定为Nib或者Code时,不可省缺
FooterHeight:   // 全局Footer的高度,省缺时默认0.01

内容数组
Content:[]
    # Content
    Content:[
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
         // 原生Cell
         Image:
         Title:
         Detail:
         Accessory:  // None/DisclosureIndicator/DisclosureButton/CheckMark/DetailButton,省缺时使用None
     },{...},{...}
    ]

样式
Style:{}    // 样式表
    Style:{
        $UIView$          // Table
        Header:(UILabel), // 原生的Header
        Footer:(UILabel), // 原生的Footer
        Row:{
            // 原生Cell
            Title:(UILabel)
            Detail:(UILabel)
            $UIView$
        }
    }
*/

#import <UIKit/UIKit.h>
#import "XXshellProtocol.h"

NS_ASSUME_NONNULL_BEGIN

#define TABLEVIEW_SIG_SELECTED          @"sigSelected"
//#define TABLEVIEW_SIG_SELECTCHANGED     @"sigSelectChanged"

@interface UITableView(XXshell)<XXshellProtocol>
@end

NS_ASSUME_NONNULL_END

