//
//  WebViewPayShell.h
//  CocoaAsyncSocket
//
//  Created by VINSON on 2020/3/27.
//

#import <WebKit/WebKit.h>
#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN

@interface WebViewPayShell : NSObject<WKNavigationDelegate,WKScriptMessageHandler>
@property (nonatomic,copy) NSString *bundleURLSchemes;  // app的标识（与微信商家标识相同）
@property (nonatomic,copy) void(^onBack)(void);         // 返回上一个vc页面
@property (nonatomic,copy,nullable) void(^onTitleChanged)(NSString *title);
@property (nonatomic,copy,nullable) void(^onReceiveScripMessage)(NSString *name);

- (void)shellWebView:(WKWebView*)webView
        goBackButton:(UIButton*)goBack
        progressView:(UIProgressView*)progress;
- (WKWebView*)shellConfiguration:(WKWebViewConfiguration*)configuration
                    goBackButton:(UIButton*)goBack
                    progressView:(UIProgressView*)progress;
- (void)loadRoot:(NSString*)url;
- (void)onGoBackButtonTouchUpInside;
@end

NS_ASSUME_NONNULL_END
