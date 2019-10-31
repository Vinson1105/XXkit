/**
 general:{}     // 一次性配置数据
 content:{}     // 容器型内容数据
 style:{}       // 样式配置(一般不包括坐标/大小)
 当没有general关键字,则认为是general的配置
 */
#import <Foundation/Foundation.h>

#define XXTR(string) ([string hasSuffix:@"??"] ? NSLocalizedString([string substringToIndex:[string length]-2],nil) : string)

NS_ASSUME_NONNULL_BEGIN

@protocol XXshellProtocol
/** 配置控件:general+content+style,没有general关键字是,认为数据为general,其中content和style */
@required -(void)generalWithJString:(NSString*)jstring;
@required -(void)generalWithJDictionary:(NSDictionary*)jdict;

/** 行为触发 */
@required -(void)actionWithJDictionary:(NSDictionary*)jdict;

/** 容器内容设置 */
@optional -(void)contentWithtJString:(NSString*)jstring;
@optional -(void)contentWithJArray:(NSArray*)jarray;

/** 样式设置 */
@optional -(void)styleWithJString:(NSString*)jstring;
@optional -(void)styleWithJDictionary:(NSDictionary*)jdict;
@end

NS_ASSUME_NONNULL_END
