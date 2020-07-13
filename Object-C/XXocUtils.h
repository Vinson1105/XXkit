/**
 2020.04.27
 1、增加JSON和NSString转换接口
 2、增加Date和NSString转换接口
 3、增加基本文档路径获取
 
 2020.04.26
 1、增加Hex转UIColor接口
 2、增加UIAlertController简单生成接口
 
 2019.01.09 - 基本工具类
 */

#ifndef __XXocUtils_h
#define __XXocUtils_h

//// 调试信息输出(注意：由于__FILE__并不是输出文件名称而是路径，调试信息输出可能过长，需要再添加部分处理以输出文件名称)（废弃）
//#ifdef XX_DEBUG
//#define XXNSLog(...) NSLog(@"[XX] [FILE:%s] [LINE:%d] %@", __FILE__, __LINE__, [NSString stringWithFormat:__VA_ARGS__])
//#else
//#define XXNSLog(...) {}
//#endif

#define XXOC_WS             __weak typeof(self) ws = self;
#define XXOC_SS             __strong typeof(ws) ss = ws;
#define XXOC_SCREEN_WIDTH   ([[UIScreen mainScreen] bounds].size.width)     // 屏幕宽度
#define XXOC_SCREEN_HEIGHT  ([[UIScreen mainScreen] bounds].size.height)    // 屏幕高度

#define XXOC_IS_KINDOF(obj,cls) (nil != obj && [obj isKindOfClass:[cls class]])
#define XXOC_IS_STRING(obj)     XXOC_IS_KINDOF(obj,NSString)
#define XXOC_IS_DICTIONARY(obj) XXOC_IS_KINDOF(obj,NSDictionary)
#endif

#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN

@interface XXocUtils : NSObject
#pragma mark - <UIViewCotroller>
+ (UIViewController*)viewController:(NSString*)storyboardID;

#pragma mark - <NSLayoutConstraint>
+ (void)view:(UIView*)view size:(CGSize)size;
+ (void)view:(UIView*)view margin:(CGFloat)margin fillAt:(UIView*)fillAt;
+ (void)view:(UIView*)view centerAt:(UIView*)centerAt;
+ (void)view:(UIView*)view left:(CGFloat)left centerYAt:(UIView*)centerYAt;
+ (void)view:(UIView*)view right:(CGFloat)right centerYAt:(UIView*)centerYAt;

#pragma mark - <UIButton>
+ (void)button:(UIButton*)button norImg:(UIImage*)norImg selImg:(UIImage*)selImg;
+ (void)button:(UIButton*)button norImg:(UIImage*)norImg disImg:(UIImage*)disImg;
+ (void)button:(UIButton*)button norImg:(UIImage*)norImg selImg:(UIImage*)selImg disImg:(UIImage*)disImg;
+ (void)button:(UIButton*)button norImg:(UIImage*)norImg selImg:(UIImage*)selImg disNorImg:(UIImage*)disNorImg disSelImg:(UIImage*)disSelImg;

+ (void)button:(UIButton*)button norTxt:(NSString*)norTxt selTxt:(NSString*)selTxt;
+ (void)button:(UIButton*)button norTxt:(NSString*)norTxt disTxt:(NSString*)disTxt;
+ (void)button:(UIButton*)button norTxt:(NSString*)norTxt selTxt:(NSString*)selTxt disTxt:(NSString*)disTxt;
+ (void)button:(UIButton*)button norTxt:(NSString*)norTxt selTxt:(NSString*)selTxt disNorTxt:(NSString*)disNorTxt disSelTxt:(NSString*)disSelTxt;

#pragma mark - <UIColor>
+ (UIColor*)colorFromHexString:(NSString*)hexString;
+ (UIColor*)colorFromHexString:(NSString*)hexString alpha:(CGFloat)alpha;

#pragma mark - <UIAlertController>
+ (UIAlertController*)alertWithTitle:(NSString*)title
                                 msg:(NSString*)msg
                             okTitle:(nullable NSString*)okTitle
                                onOK:(nullable void (^)(UIAlertAction *action))onOK
                         cancelTitle:(nullable NSString*)cancelTitle
                            onCancel:(nullable void (^)(UIAlertAction *action))onCancel;

+ (void)alert:(UIAlertController*)alert
      okTitle:(nullable NSString*)okTitle
         onOK:(nullable void (^)(UIAlertAction *action))onOK
  cancelTitle:(nullable NSString*)cancelTitle
     onCancel:(nullable void (^)(UIAlertAction *action))onCancel;

#pragma mark - <JSON>
+ (nullable NSString*)jsonStringWithJson:(id)json pretty:(BOOL)pretty;
+ (nullable id)jsonWithJsonString:(NSString*)jsonString;

#pragma mark - <Date>
+ (void)setDefaultDateFormatter:(NSDateFormatter*)formatter;
+ (NSString*)currentDateString;
+ (NSString*)currentDateStringWithDateFormat:(NSString*)format timeZone:(NSTimeZone*)timeZone;
+ (NSString*)dateStringWithTimestamp:(NSTimeInterval)timestamp;
+ (NSString*)dateStringWithTimestamp:(NSTimeInterval)timestamp dateFormat:(NSString*)format timeZone:(NSTimeZone*)timeZone;

#pragma mark - <Time>
+ (NSString*)timeStringWithSecond:(NSTimeInterval)second timeFormat:(NSString*)format;

#pragma mark - <File System>
/** 返回沙箱文档文件夹路径 */
+ (NSString*)documentAbsolutePathString;
+ (NSURL*)documentAbsolutePathUrl;
/** 返回沙箱文档中以若干个节点组成的路径 */
+ (NSString*)absolutePathStringInDocument:(NSArray*)nodes;
+ (NSURL*)absolutePathUrlInDocument:(NSArray*)nodes;
/** 在沙箱文档中以若干个节点组成的路径下创建文件夹 */
+ (nullable NSString*)mkdirInDocument:(NSArray*)nodes error:(NSError**)error;

#pragma mark - <TouchID/FaceID>
/**
 使用指定reason显示TouchID/FaceID验证
 @param reason alert弹框的message
 @param reply 验证结果回调，其中error.code参考LAError的枚举值
 @return 调用成功返回YES，否则返回NO
 */
+ (BOOL)evaluatePolicyWithReason:(NSString*)reason reply:(void(^)(BOOL success, NSError * _Nullable error))reply;

#pragma mark - <Thread>
+ (void)mainThreadProcess:(void(^)(void))handler;

#pragma mark - <Bundle>
+ (NSBundle*)bundleNamed:(NSString*)name;

#pragma mark - <Run Time>
+ (void)replaceMethod:(Class)cls src:(SEL)src dest:(SEL)dest;

#pragma mark - <Audio/Video>
+ (NSTimeInterval)audioDuration:(NSURL*)url;

#pragma mark - <Image>
+ (UIImage*)imageFromColor:(UIColor*)color size:(CGSize)size;

#pragma mark - <权限>
+ (void)anthorizedCameraCheckAtViewController:(UIViewController*)viewController
                                        title:(NSString*)title
                                      message:(NSString*)message
                                      succeed:(void(^)(void))succeed;
@end

NS_ASSUME_NONNULL_END
