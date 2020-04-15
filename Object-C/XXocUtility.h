/**
 * XX库下的Object-C一些常用宏
 *
 * 2020-04-15
 * 本文件内容从单纯的宏定义更改为一些常用工具类
 *
 * 2020-04-11
 * 本文件将会被废弃
 *
 * 创建于2019-01-09
 */

#ifndef __XXocUtility_h
#define __XXocUtility_h

//// 调试信息输出(注意：由于__FILE__并不是输出文件名称而是路径，调试信息输出可能过长，需要再添加部分处理以输出文件名称)（废弃）
//#ifdef XX_DEBUG
//#define XXNSLog(...) NSLog(@"[XX] [FILE:%s] [LINE:%d] %@", __FILE__, __LINE__, [NSString stringWithFormat:__VA_ARGS__])
//#else
//#define XXNSLog(...) {}
//#endif

#define XXOC_WS     __weak typeof(self) ws = self
#define XXOC_SS     __strong typeof(ws) ss = ws;
#define XXOC_SCREEN_WIDTH   ([[UIScreen mainScreen] bounds].size.width)     // 屏幕宽度
#define XXOC_SCREEN_HEIGHT  ([[UIScreen mainScreen] bounds].size.height)    // 屏幕高度

#define XXOC_IS_KINDOF(obj,cls) (nil != obj && [obj isKindOfClass:[cls class]])
#define XXOC_IS_STRING(obj)     XXOC_IS_KINDOF(obj,NSString)
#define XXOC_IS_DICTIONARY(obj) XXOC_IS_KINDOF(obj,NSDictionary)

//// 一些在K-V中有特殊作用的键
///// XXshell中使用
//#define XXSHELL_DEFAULE_CELL        @"_DefaultCell_"    // 默认cell类型
//#define XXSHELL_DEFAULE_HEADER      @"_DefaultHeader_"  // 默认header类型
//#define XXSHELL_DEFAULE_FOOTER      @"_DefaultFooter_"  // 默认footer类型
//
//#define XXSHELL_SECTION_HEADER      @"_Header_"         // section中header节点值
//    #define XXSHELL_HEADER_TITLE    @"_Title_"          // header中的标题
//
//#define XXSHELL_SECTION_FOOTER      @"_Footer_"         // section中footer节点值
//    #define XXSHELL_FOOTER_TITLE    @"_Title_"          // footer中的标题
//
//#define XXSHELL_SECTION_ROW         @"_Row_"            // section中row节点值
//    #define XXSHELL_ROW_TEXT        @"_Text_"           // row中的text
//
///// XXstyle中使用
//#define XXSTYLE_TEXT_COLOR          @"_TextColor_"
//#define XXSTYLE_FONT_FAMILY         @"_FontFamily_"
//#define XXSTYLE_FONT_SIZE           @"_FontSize_"
//#define XXSTYLE_BACKGROUND_COLOR    @"_BackgroundColor_"
//#define XXSTYLE_BACKGROUND_IMAGE    @"_BackgroundImage_"
#endif

@import <UIKit/UIKit.h>

@interface XXocUtility
+ (UIViewController*)viewController:(NSString*)vc WithUIStoryboard:(NSString*)storyboard bundle:(nullable NSBundle*)bundle;
@end
