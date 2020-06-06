#import "UIView+MaskView.h"
#import "../View/XXmaskView.h"

@implementation UIView(MaskView)
- (void)setXx_hiddenWithMaskView:(BOOL)xx_hiddenWithMaskView{
    self.hidden = xx_hiddenWithMaskView;
    
    if(!xx_hiddenWithMaskView){
        [[XXmaskView sharedInstance] show:self animated:YES];
    }
    else{
        [XXmaskView sharedInstance].hidden = YES;
    }
}
- (BOOL)xx_hiddenWithMaskView{
    return self.hidden;
}
@end
