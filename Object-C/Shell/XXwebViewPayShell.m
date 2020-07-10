#import "XXwebViewPayShell.h"

#define kAppStoreAlipayUrl  @"https://itunes.apple.com/cn/app/id333206289?mt=8"
#define kAppStoreWXUrl      @"https://itunes.apple.com/cn/app/id414478124?mt=8"
#define kWXpayResultSuffix  @"wxpay-redirect-url" // TODO: 好像没有作用，请考虑移除

@interface XXwebViewPayShell()
@property (nonatomic,strong) WKUserContentController *userContentController;
@property (nonatomic,strong) WKWebViewConfiguration *webViewConfiguration;
@property (nonatomic,strong) NSMutableArray *urlStack;

@property (nonatomic,strong) UIButton *goBackButton;
@property (nonatomic,strong) UIProgressView *progressView;

@property (nonatomic,assign) BOOL isWXpay;              // 是否正在使用微信支付
@property (nonatomic,assign) BOOL isAlipay;             // 是否正在使用支付宝支付
@property (nonatomic,copy) NSString *rootUrl;           // 根页的完整路径
@property (nonatomic,copy) NSString *rootUrlSuffix;     // 根页不携带参数路径，如果完整根页不带参数，这个等同rootUrl
@property (nonatomic,copy) NSString *wxpayReturnRefreshUrl;

@property (nonatomic,assign) NSInteger indexForBeforePay;
@end

@implementation XXwebViewPayShell
#pragma mark - 构建析构
- (void)dealloc{
    if(_webView){
        [_webView removeObserver:self forKeyPath:@"estimatedProgress"];
        [_webView removeObserver:self forKeyPath:@"title"];
    }
}

#pragma mark - 公共接口
- (void)shell:(WKWebView*)webView goBackButton:(UIButton*)goBack progressView:(UIProgressView*)progress{
    if(nil != _webView){
        NSLog(@"[XXwebViewPayShell] webView is existed, can not set again.");
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
    
    _indexForBeforePay = -1;
    _isWXpay = NO;
    _isAlipay = NO;
}
- (WKWebView*)shellConfiguration:(WKWebViewConfiguration*)configuration
                    goBackButton:(UIButton*)goBack
                    progressView:(UIProgressView*)progress{
    if(nil != _webView){
        NSLog(@"[XXwebViewPayShell] webView is existed, can not set again.");
        return _webView;
    }
    WKWebView *webView = [[WKWebView alloc] initWithFrame:[UIApplication sharedApplication].delegate.window.bounds configuration:configuration];
    [self shell:webView goBackButton:goBack progressView:progress];
    return webView;
}
- (void)loadRoot:(NSString*)url{
    self.rootUrl = url;
    if([url containsString:@"?"]){
        NSRange range = [url rangeOfString:@"?"];
        range.length = range.location;
        range.location = 0;
        self.rootUrlSuffix = [url substringWithRange:range];
    }
    else{
        self.rootUrlSuffix = url;
    }
    NSURLRequest *request =[NSURLRequest requestWithURL:[NSURL URLWithString:url] cachePolicy:NSURLRequestReloadIgnoringLocalCacheData timeoutInterval:6.0];
    [_webView loadRequest:request];
}
- (void)reloadRoot{
    NSURLRequest *request =[NSURLRequest requestWithURL:[NSURL URLWithString:_rootUrl] cachePolicy:NSURLRequestReloadIgnoringLocalCacheData timeoutInterval:6.0];
    [_webView loadRequest:request];
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
        _onTitleChanged(self,_webView.title);
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
        _onReceiveScripMessage(self,message.name);
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
    NSString *urlString = [navigationAction.request.URL.absoluteString stringByRemovingPercentEncoding];
    NSLog(@"\n[XXwebViewPayShell] (backcount:%d) %@", webView.backForwardList.backList.count, urlString);
    
    if([urlString hasPrefix:_rootUrlSuffix]){
        self.indexForBeforePay = -1;
        self.isWXpay = NO;
        self.isAlipay = NO;
        decisionHandler(WKNavigationActionPolicyAllow);
    }
    else if([urlString hasPrefix:@"https://openapi.alipay.com"]){
        if(self.isAlipay){
            // 支付宝页面中左上角区域的手动关闭返回，拦截并取消请求，否则会再次进入支付宝页面
            decisionHandler(WKNavigationActionPolicyCancel);
            [self goBackBeforePay];
        }
        else{
            // 进入支付宝调用
            decisionHandler(WKNavigationActionPolicyAllow);
            self.isAlipay = YES;
            self.isWXpay = NO;
            
            // 记录进入支付宝的页编号，当前有两种可能触发这个url，一个是进入支付宝支付，一个是点击支付宝的左上角的关闭，但是这两情况的listCount不一样，所以只是以进入的优先
            if(_indexForBeforePay<0){
                _indexForBeforePay = webView.backForwardList.backList.count-1;
            }
        }
    }
    else if([urlString isEqualToString:kAppStoreAlipayUrl]){
        // 进入App Store中的支付宝页面
        decisionHandler(WKNavigationActionPolicyCancel);
        [[UIApplication sharedApplication] openURL:[NSURL URLWithString:kAppStoreAlipayUrl]];
    }
    else if ([urlString hasPrefix:@"alipay://"]) {
        // 支付宝支付链接中没有重定向所用的app url，需要手动增加
        decisionHandler(WKNavigationActionPolicyCancel);
        if(![urlString containsString:_bundleURLSchemes]){
            urlString = [urlString stringByReplacingOccurrencesOfString:@"\"fromAppUrlScheme\":\"alipays\"" withString:[NSString stringWithFormat:@"\"fromAppUrlScheme\":\"%@\"",_bundleURLSchemes]];
            urlString = [urlString stringByAddingPercentEncodingWithAllowedCharacters:[NSCharacterSet URLQueryAllowedCharacterSet]];
            NSURL *url = [NSURL URLWithString:urlString];
            if(nil == url){
                NSLog(@"[XXwebViewPayShell] alipay url is nil after append bundleURLSchemes");
            }
        }
        
        if ([[UIApplication sharedApplication] canOpenURL:[NSURL URLWithString:urlString]]) {
            // 已安装支付宝
            [[UIApplication sharedApplication] openURL:[NSURL URLWithString:urlString]];
        }
        else {
            // 未安装支付宝
            [[UIApplication sharedApplication] openURL:[NSURL URLWithString:kAppStoreAlipayUrl]];
        }
    }
    else if ([urlString hasPrefix:@"https://wx.tenpay.com"]){
        // 微信支付链接中没有重定向所用的app url，需要手动增加
        NSMutableString *newUrlString = [[NSMutableString alloc]init];
        if(![urlString containsString:[NSString stringWithFormat:@"redirect_url=%@://", _bundleURLSchemes]]) {
            NSRange range = [urlString rangeOfString:@"&redirect_url="];
            self.wxpayReturnRefreshUrl = [[NSString alloc] initWithUTF8String:urlString.UTF8String+range.location+range.length];

            NSArray *strArray = [urlString componentsSeparatedByString:@"&redirect_url"];
            [newUrlString appendString:strArray.firstObject];
            [newUrlString appendString:[NSString stringWithFormat:@"&redirect_url=%@://", _bundleURLSchemes]];
            // 拦截微信,增加重定向schemeurl,并重新请求
            decisionHandler(WKNavigationActionPolicyCancel);
            NSMutableURLRequest *mwebRequest = [NSMutableURLRequest requestWithURL:[NSURL URLWithString:newUrlString]];
            mwebRequest.allHTTPHeaderFields = navigationAction.request.allHTTPHeaderFields;
            [mwebRequest setValue:[NSString stringWithFormat:@"%@://",_bundleURLSchemes] forHTTPHeaderField:@"Referer"];
            [self.webView loadRequest:mwebRequest];
            
            // 记录进入微信支付的页编号
            if(_indexForBeforePay<0){
                _indexForBeforePay = webView.backForwardList.backList.count;
            }
        }
        else {
            // 已经包含，直接运行进入微信app
            decisionHandler(WKNavigationActionPolicyAllow);
            self.isWXpay = YES;
            self.isAlipay = NO;
        }
    }
    else if ([urlString hasPrefix:@"weixin://"]) {
        // 调起微信
        decisionHandler(WKNavigationActionPolicyCancel);
        if(![[UIApplication sharedApplication] openURL:navigationAction.request.URL]){
            // 没有安装微信,跳转到App Store下载微信
            [[UIApplication sharedApplication] openURL:[NSURL URLWithString:kAppStoreWXUrl]];
            self.isWXpay = YES;
            self.isAlipay = NO;
        }
    }
    else if ([urlString hasPrefix:_bundleURLSchemes]){
        // 支付完成返回到app，重新请求一下获取订单状态（暂时好像只有微信支付会触发这个）
        if (self.isWXpay){
            if (self.wxpayReturnRefreshUrl) {
                decisionHandler(WKNavigationActionPolicyCancel);
                [self.webView loadRequest:[NSURLRequest requestWithURL:[NSURL URLWithString:self.wxpayReturnRefreshUrl]]];
            }else {
                decisionHandler(WKNavigationActionPolicyAllow);
            }
        }
        else {
           decisionHandler(WKNavigationActionPolicyAllow);
        }
    }
    else {
        decisionHandler(WKNavigationActionPolicyAllow);
    }
    
}

- (void)onGoBackButtonTouchUpInside{
    if(self.onGoBack && !self.onGoBack(self)){
        return;
    }
    
    NSString *urlString = _webView.URL.absoluteString;
    if(nil == _webView.URL || [urlString isEqualToString:_rootUrl] || [urlString hasPrefix:_rootUrlSuffix]){
        UIViewController *viewController = [self viewController:_webView];
        if(viewController){
            [viewController.navigationController popViewControllerAnimated:YES];
        }
    }
    else if([_webView.URL.absoluteString containsString:kWXpayResultSuffix]){
        [_webView loadRequest:[NSURLRequest requestWithURL:[NSURL URLWithString:_rootUrl]]];
    }
    else if(self.indexForBeforePay>=0){
        [self goBackBeforePay];
    }
    else{
        [_webView goBack];
    }
}

-(UIViewController *)viewController:(UIView*)view{
    UIViewController *viewController = nil;
    UIResponder *next = view.nextResponder;
    while (next) {
        if ([next isKindOfClass:[UIViewController class]]) {
            viewController = (UIViewController *)next;
            break;
        }
        next = next.nextResponder;
    }
    return viewController;
}
- (void)goBackBeforePay{
    NSArray *backList =  _webView.backForwardList.backList;
    if(_indexForBeforePay<0 || _indexForBeforePay>=backList.count){
        NSLog(@"[XXwebViewPayShell] [gotoBeforePay] _indexForBeforePay:%ld backList.count:%lu",
              (long)_indexForBeforePay,
              (unsigned long)backList.count);
        return;
    }
    
    [_webView goToBackForwardListItem:backList[_indexForBeforePay]];
    _indexForBeforePay = -1;
    _isWXpay = NO;
    _isAlipay = NO;
}
@end
