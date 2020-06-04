#import "XXbuttonPageShiftShell.h"

@interface XXbuttonPageShiftShell()
@property (nonatomic,assign) SEL func;
@end

#pragma mark - 构建析构
@implementation XXbuttonPageShiftShell
#pragma mark - <Init>
- (instancetype)init{
    self = [super init];
    if (self) {
        _current = 1;
        _count  = 0;
        _format = @"%d/%d";
    }
    return self;
}

#pragma mark - <Shell>
-(void)shellWithPrev:(UIButton*)prev next:(UIButton*)next page:(nullable UILabel*)page{
    /// 有效性判断
    if(!prev || !next){
        NSLog(@"[XXbuttonPageShiftShell] [shellWithPrev: next: page:] nil object can not be shell. (prev:%@ next:%@)",prev,next);
        return;
    }
    
    /// 移除原来控件
    if(self.prevButton) {
        [self.prevButton removeTarget:self action:@selector(toPrev) forControlEvents:UIControlEventTouchUpInside];
    }
    if(self.nextButton){
        [self.nextButton removeTarget:self action:@selector(toNext) forControlEvents:UIControlEventTouchUpInside];
    }
    
    /// 重置属性
    _prevButton = prev;
    _nextButton = next;
    _pageLabel = page;
    
    [self.prevButton addTarget:self action:@selector(toPrev) forControlEvents:UIControlEventTouchUpInside];
    [self.nextButton addTarget:self action:@selector(toNext) forControlEvents:UIControlEventTouchUpInside];
}
- (void)resetCurrent:(int)current count:(int)count{
    if(current<=0 || current>count){
        NSLog(@"[XXbuttonPageShiftShell] [resetCurrent: count:] page range is invalid. (current:%d count:%d)",current,count);
        return;
    }
    
    _current = current;
    _count = count;
    [self updatePage];
}

#pragma mark - <上下页>
- (void)toPrev{
    if(0==_count || _current<=1) return;
    --_current;
    [self updatePage];
    if(_onPageTouchChanged){
        _onPageTouchChanged(self,_current+1,_current);
    }
}
- (void)toNext{
    if(0==_count || _current>=_count) return;
    ++_current;
    [self updatePage];
    if(_onPageTouchChanged){
        _onPageTouchChanged(self,_current-1,_current);
    }
}
- (void)updatePage{
    if(self.pageLabel) _pageLabel.text = [NSString stringWithFormat:_format,_current,_count];
    if(self.prevButton) _prevButton.enabled = !(_current<=1);
    if(self.nextButton) _nextButton.enabled = !(_current>=_count);
}
@end
