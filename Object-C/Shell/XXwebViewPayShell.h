/**
 2020.03.27
 支付宝、微信支付拦截、调用等系列操作
 需要在info.plist添加以下标识，&self.bundleURLSchemes&表示一个引用值
 <key>CFBundleURLTypes</key>
 <array>
 <dict>
    ...
 </dict>
 <dict>
     <key>CFBundleTypeRole</key>
     <string>Editor</string>
     <key>CFBundleURLSchemes</key>
     <array>
        <string>&self.bundleURLSchemes&</string>
     </array>
 </dict>
 </array>
 */

#import <WebKit/WebKit.h>
#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN

@interface XXwebViewPayShell : NSObject<WKNavigationDelegate,WKScriptMessageHandler>
@property (nonatomic,strong,readonly) WKWebView *webView;
@property (nonatomic,copy) NSString *bundleURLSchemes;  // app的url标识，需要在Project中的info(info.plist)中添加对应的标识（与微信商家标识相同，不带http://）

@property (nonatomic,copy,nullable) void(^onTitleChanged)(XXwebViewPayShell *shell, NSString *title);       // title改变
@property (nonatomic,copy,nullable) void(^onReceiveScripMessage)(XXwebViewPayShell *shell, NSString *name); // 暂时没有使用
@property (nonatomic,copy,nullable) BOOL(^onGoBack)(XXwebViewPayShell *shell);                              // 由返回按键触发的页面返回动作，返回YES执行内置判断操作

/**
 配置
 @param webView 目标WKWebView
 @param goBack 页面后退按键
 @param progress 页面加载进度条
 */
- (void)shell:(WKWebView*)webView goBackButton:(UIButton*)goBack progressView:(UIProgressView*)progress;

/**
 创建一个WKWebView并返回
 @param configuration 创建WKWebView需要的配置
 @param goBack 页面后退按键
 @param progress 页面加载进度条
 */
- (WKWebView*)shellConfiguration:(WKWebViewConfiguration*)configuration
                    goBackButton:(UIButton*)goBack
                    progressView:(UIProgressView*)progress;

/**
 加载首页
 @param url 完整的首页url
 */
- (void)loadRoot:(NSString*)url;

/**
 重新加载首页，调用此函数之前需要先使用loadRoot
 */
- (void)reloadRoot;

/**
 返回支付前一页
 */
- (void)goBackBeforePay;
@end

NS_ASSUME_NONNULL_END
