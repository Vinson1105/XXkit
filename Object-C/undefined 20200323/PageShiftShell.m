//
//  PageShiftShell.m
//  iCamSee
//
//  Created by VINSON on 2019/11/19.
//  Copyright © 2019 Macrovideo. All rights reserved.
//

#import "PageShiftShell.h"

#define PAGE_TITLE(current,count) [NSString stringWithFormat:@"%d/%d", current, count]

@interface PageShiftShell()
@property (nonatomic,assign) SEL func;
@end

#pragma mark - 构建析构
@implementation PageShiftShell
- (instancetype)init{
    self = [super init];
    if (self) {
        _currentPage = 1;
        _pageCount = 0;
        _pageBeginFromZore = NO;
    }
    return self;
}

#pragma mark - 公共接口: <shell>
-(void)shellWithLeft:(UIButton*)left Right:(UIButton*)right Page:(UILabel*)page{
    _leftButton = left;
    _rightButton = right;
    _pageLabel = page;
    
    //__strong UIButton *strongLeft = s;
    [self.leftButton addTarget:self action:@selector(toLeft) forControlEvents:UIControlEventTouchUpInside];
    [self.rightButton addTarget:self action:@selector(toRight) forControlEvents:UIControlEventTouchUpInside];
}

#pragma mark - 属性读写: <页数量> <当前页> <是否从页0开始>
- (void)setPageCount:(int)pageCount{
    if(_pageCount == pageCount) return;
    _pageCount = pageCount;
    [self updatePage];
}
- (void)setCurrentPage:(int)currentPage{
    if(_currentPage == currentPage) return;
    _currentPage = currentPage;
    [self updatePage];
}
- (void)setPageBeginFromZore:(BOOL)pageBeginFromZore{
    if(_pageBeginFromZore == pageBeginFromZore) return;
    _pageBeginFromZore ? self.currentPage-- : self.currentPage++;
    _pageBeginFromZore = pageBeginFromZore;
}

#pragma mark - 私有函数: <左> <右> <左边界检测> <右边界检测> <更新>
- (void)toLeft{
    if(0 == _pageCount) return;
    self.currentPage--;
    if(_onTouchPageChanged){
        _onTouchPageChanged(_currentPage+1,_currentPage);
    }
}
- (void)toRight{
    if(0 == _pageCount) return;
    self.currentPage++;
    if(_onTouchPageChanged){
        _onTouchPageChanged(_currentPage-1,_currentPage);
    }
}
- (BOOL)isLeftBoundary{
    return _pageBeginFromZore ? 0 == _currentPage : 1 == _currentPage;
}
- (BOOL)isRightBoundary{
    return _pageBeginFromZore ? _currentPage >= _pageCount-1 : _currentPage >= _pageCount;
}
- (void)updatePage{
    if(self.pageLabel) _pageLabel.text = PAGE_TITLE(_currentPage,_pageCount);
    if(self.leftButton) _leftButton.enabled = ![self isLeftBoundary];
    if(self.rightButton) _rightButton.enabled = ![self isRightBoundary];
}
@end
