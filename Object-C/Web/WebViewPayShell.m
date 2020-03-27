//
//  WebViewPayShell.m
//  CocoaAsyncSocket
//
//  Created by VINSON on 2020/3/27.
//

#import "WebViewPayShell.h"

#define kAppStoreAlipayUrl  @"itms-appss://apps.apple.com/cn/app/id333206289"
#define kAppStoreWXUrl      @"https://itunes.apple.com/cn/app/id414478124?mt=8"
#define kWXpayReturnSuffix  @"wxpay-redirect-url"

@interface WebViewPayShell()
@property (nonatomic,strong) WKWebView *webView;
@property (nonatomic,strong) WKUserContentController *userContentController;
@property (nonatomic,strong) WKWebViewConfiguration *webViewConfiguration;

@property (nonatomic,strong) UIButton *goBackButton;
@property (nonatomic,strong) UIProgressView *progressView;

@property (nonatomic,assign) BOOL isWXpay;
@property (nonatomic,assign) BOOL isAlipay;
@property (nonatomic,copy) NSString *rootUrl;
@property (nonatomic,copy) NSString *wxpayReturnRefreshUrl;
@end

@implementation WebViewPayShell
#pragma mark - 构建析构
- (void)dealloc{
    if(_webView){
        [_webView removeObserver:self forKeyPath:@"estimatedProgress"];
        [_webView removeObserver:self forKeyPath:@"title"];
    }
}

#pragma mark - 公共接口
- (void)shellWebView:(WKWebView *)webView
        goBackButton:(UIButton *)goBack
        progressView:(UIProgressView *)progress{
    if(nil != webView){
        NSLog(@"[WebViewPayShell] webView is existed, can not set again.");
        return;
    }
    
    _webView = webView;
    _webView.navigationDelegate = self;
    //添加监测网页加载进度的观察者
    [_webView addObserver:self
               forKeyPath:@"estimatedProgress"
                  options:0
                  context:nil];
    //添加监测网页标题title的观察者
    [_webView addObserver:self
               forKeyPath:@"title"
                  options:NSKeyValueObservingOptionNew
                  context:nil];
    
    _goBackButton = goBack;
    [_goBackButton addTarget:self action:@selector(onGoBackButtonTouchUpInside) forControlEvents:UIControlEventTouchUpInside];
    
    _progressView = progress;
    _progressView.hidden = YES;
}
- (WKWebView*)shellConfiguration:(WKWebViewConfiguration*)configuration
                    goBackButton:(UIButton*)goBack
                    progressView:(UIProgressView*)progress{
    if(nil != _webView){
        NSLog(@"[WebViewPayShell] webView is existed, can not set again.");
        return _webView;
    }
    WKWebView *webView = [[WKWebView alloc] initWithFrame:CGRectMake(0, 0, 100, 100) configuration:configuration];
    [self shellWebView:webView goBackButton:goBack progressView:progress];
    return webView;
}
- (void)loadRoot:(NSString*)url{
    _rootUrl = url;
    [_webView loadRequest:[NSURLRequest requestWithURL:[NSURL URLWithString:url]]];
}

#pragma mark - 私有函数: <观察者>
-(void)observeValueForKeyPath:(NSString *)keyPath
                     ofObject:(id)object
                       change:(NSDictionary<NSKeyValueChangeKey,id> *)change
                      context:(void *)context{
    if ([keyPath isEqualToString:NSStringFromSelector(@selector(estimatedProgress))]
        && object == _webView) {
        self.progressView.progress = _webView.estimatedProgress;
    }
    else if([keyPath isEqualToString:@"title"]
             && object == _webView && _onTitleChanged){
        _onTitleChanged(_webView.title);
    }
    else{
        [super observeValueForKeyPath:keyPath
                             ofObject:object
                               change:change
                              context:context];
    }
}

#pragma mark - 协议函数: <WKScriptMessageHandler>
- (void)userContentController:(nonnull WKUserContentController *)userContentController didReceiveScriptMessage:(nonnull WKScriptMessage *)message {
    if(_onReceiveScripMessage){
        _onReceiveScripMessage(message.name);
    }
}

#pragma mark - 协议函数: <WKNavigationDelegate>
- (void)webView:(WKWebView *)webView didStartProvisionalNavigation:(WKNavigation *)navigation{
    _progressView.hidden = NO;
}
- (void)webView:(WKWebView *)webView didFinishNavigation:(WKNavigation *)navigation{
    _progressView.hidden = YES;
}
- (void)webView:(WKWebView *)webView decidePolicyForNavigationAction:(WKNavigationAction *)navigationAction decisionHandler:(void (^)(WKNavigationActionPolicy))decisionHandler{
    NSString *urlString = webView.URL.absoluteString;
    
    if([urlString isEqualToString:_rootUrl]){
        // 首页
        decisionHandler(WKNavigationActionPolicyAllow);
        _isAlipay = NO;
        _isWXpay = NO;
    }
    else if([urlString hasPrefix:@"alipays://"] || [urlString hasPrefix:@"alipay://"]){
        // 支付宝
        if([[UIApplication sharedApplication] canOpenURL:[NSURL URLWithString:@"alipay://"]]){
            // 已安装支付宝
            if([urlString containsString:_bundleURLSchemes]){
                // 已经包含本app的跳转标识，直接允许跳转
                decisionHandler(WKNavigationActionPolicyAllow);
            }
            else{
                // 未包含本app的跳转标识，拦截并加入标识
                urlString = [urlString stringByReplacingOccurrencesOfString:@"\"fromAppUrlScheme\":\"alipays\"" withString:[NSString stringWithFormat:@"\"fromAppUrlScheme\":\"%@\"",_bundleURLSchemes]];
                urlString = [urlString stringByAddingPercentEncodingWithAllowedCharacters:[NSCharacterSet URLQueryAllowedCharacterSet]];
                NSURL *url = [NSURL URLWithString:urlString];
                if(nil == url){
                    NSLog(@"[WebViewPayShell] alipay url is nil after append bundleURLSchemes");
                }
                [[UIApplication sharedApplication] openURL:url];
                decisionHandler(WKNavigationActionPolicyCancel);
            }
        }
        else{
            // 未安装支付宝，跳转到App Store中
            [[UIApplication sharedApplication] openURL:[NSURL URLWithString:kAppStoreAlipayUrl]];
            decisionHandler(WKNavigationActionPolicyCancel);
        }
    }
    else if([urlString hasPrefix:@"weixin://"]){
        // 调起微信
        decisionHandler(WKNavigationActionPolicyCancel);
        if(![[UIApplication sharedApplication] openURL:navigationAction.request.URL]){
            // 没有安装微信,跳转到App Store下载微信
            [[UIApplication sharedApplication] openURL:[NSURL URLWithString:kAppStoreWXUrl]];
        }
    }
    else if([urlString hasPrefix:@"https://wx.tenpay.com"]){
        // 微信支付请求
        if(![urlString containsString:[NSString stringWithFormat:@"redirect_url=%@://", _bundleURLSchemes]]){
            // 包含重定向url，需要前面部分地址作为返回时
            NSRange range       = [urlString rangeOfString:@"&redirect_url="];
            int length          = (int)range.location;
            NSString *realUrl   = [[NSString alloc] initWithBytes:urlString.UTF8String length:length encoding:NSUTF8StringEncoding];
            _wxpayReturnRefreshUrl = [[NSString alloc] initWithUTF8String:urlString.UTF8String+range.location+range.length];
            
            // 拦截微信,增加重定向schemeurl,并重新请求
            decisionHandler(WKNavigationActionPolicyCancel);
            NSString *mwebUrlString             = [realUrl stringByAppendingFormat:@"&redirect_url=%@://", _bundleURLSchemes];
            NSMutableURLRequest *mwebRequest    = [NSMutableURLRequest requestWithURL:[NSURL URLWithString:mwebUrlString]];
            mwebRequest.allHTTPHeaderFields     = navigationAction.request.allHTTPHeaderFields;
            [mwebRequest setValue:[NSString stringWithFormat:@"%@://",_bundleURLSchemes] forHTTPHeaderField:@"Referer"];
            [webView loadRequest:mwebRequest];
        }
        else{
            decisionHandler(WKNavigationActionPolicyAllow);
            if(_isWXpay){
                dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(0.3 * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
                    [webView goBack];
                });
                _isWXpay = NO;
            }
            else{
                _isWXpay = YES;
                _isAlipay = NO;
            }
        }
    }
    else if([urlString hasPrefix:[NSString stringWithFormat:@"%@://",_bundleURLSchemes]]){
        // 从微信返回APP,需要手动刷新
        decisionHandler(WKNavigationActionPolicyCancel);
        [_webView loadRequest:[NSURLRequest requestWithURL:[NSURL URLWithString:_wxpayReturnRefreshUrl]]];
    }
    else if([urlString hasPrefix:@"itms-appss:"]){
        // 支付包页面点击下载支付宝，需要拦截，并使用application跳转
        decisionHandler(WKNavigationActionPolicyCancel);
        [[UIApplication sharedApplication] openURL:[NSURL URLWithString:urlString]];
    }
    else{
        decisionHandler(WKNavigationActionPolicyAllow);
    }
}

- (void)onGoBackButtonTouchUpInside{
    if(nil == _webView.URL || [_webView.URL.absoluteString isEqualToString:_rootUrl]){
        if(_onBack){
            _onBack();
        }
    }
    else if([_webView.URL.absoluteString containsString:kWXpayReturnSuffix]){
        [_webView loadRequest:[NSURLRequest requestWithURL:[NSURL URLWithString:_rootUrl]]];
    }
    else{
        [_webView goBack];
    }
}
@end
