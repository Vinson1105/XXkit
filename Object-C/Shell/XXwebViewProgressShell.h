/**
 2020.05.09
 集成进度条管理的WKWebView，适用于一些单页的协议、说明的url加载显示
 */

#import <WebKit/WebKit.h>
#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN

@interface XXwebViewProgressShell : NSObject<WKNavigationDelegate>
@property (nonatomic,strong,readonly) WKWebView *webView;
@property (nonatomic,strong,readonly) UIProgressView *progressView;
@property (nonatomic,copy) void(^onRequestFailure)(XXwebViewProgressShell *shell, NSError *error);

/**
 配置目标UIWebView和用于显示进度的UIProgressView
 @param webView 目标WKWebView
 @param progressView 显示进度
 */
- (void)shell:(WKWebView*)webView progress:(UIProgressView*)progressView;

/**
 页面请求
 @param url 目标页面的url
 */
- (void)request:(NSURL*)url;
@end

NS_ASSUME_NONNULL_END
