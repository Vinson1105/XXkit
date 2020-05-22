#import "XXwebViewProgressShell.h"

@implementation XXwebViewProgressShell
- (void)shell:(WKWebView*)webView progress:(UIProgressView*)progressView{
    _webView = webView;
    [_webView addObserver:self
               forKeyPath:@"estimatedProgress"
                  options:0
                  context:nil];
    _progressView = progressView;
}
- (void)dealloc{
    [_webView removeObserver:self forKeyPath:@"estimatedProgress"];
}
- (void)request:(NSURL*)url{
    NSURLRequest *request =[NSURLRequest requestWithURL:url
                                            cachePolicy:NSURLRequestReloadIgnoringLocalCacheData
                                        timeoutInterval:6.0];
    [self.webView loadRequest:request];
}

-(void)observeValueForKeyPath:(NSString *)keyPath
                     ofObject:(id)object
                       change:(NSDictionary<NSKeyValueChangeKey,id> *)change
                      context:(void *)context{
    if ([keyPath isEqualToString:NSStringFromSelector(@selector(estimatedProgress))]
        && object == _webView) {
        _progressView.progress = _webView.estimatedProgress;
        if(_webView.estimatedProgress >= 1.f){
            _progressView.hidden = YES;
        }
        else{
            _progressView.hidden = NO;
        }
        
    }else{
        [super observeValueForKeyPath:keyPath
                             ofObject:object
                               change:change
                              context:context];
    }
}

- (void)webView:(WKWebView *)webView didStartProvisionalNavigation:(WKNavigation *)navigation{
}
- (void)webView:(WKWebView *)webView didFinishNavigation:(WKNavigation *)navigation{
}
- (void)webView:(WKWebView *)webView didFailNavigation:(WKNavigation *)navigation withError:(NSError *)error{
    if(_onRequestFailure){
        _onRequestFailure(self, error);
    }
}
@end
