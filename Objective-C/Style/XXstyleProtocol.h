#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN
@protocol XXstyleProtocol
@required -(void)styleWithJString:(NSString*)jstring;
@required -(void)styleWithJDictionary:(NSDictionary*)jdicty;
@optional +(instancetype)styleWithJString:(NSString*)jstring;
@optional +(instancetype)styleWithJDictionary:(NSDictionary*)jdict;
@end
NS_ASSUME_NONNULL_END
