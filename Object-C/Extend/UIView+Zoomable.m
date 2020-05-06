#import "UIView+Zoomable.h"
#import <objc/runtime.h>

#define kZoomable   @"zoomable"
#define kScaleMax   @"scaleMax"
#define kScaleMin   @"scaleMin"

#define kTotalScale     @"totalScale"
#define kNormalX        @"normalX"
#define kNormalY        @"normalY"
#define kNormalWidth    @"normalWidth"
#define kNormalHeight   @"normalHeight"

#define kPinchGesture   @"pinchGesture"
#define kPanGesture     @"panGesture"

@interface UIView()
@property (nonatomic,assign) CGFloat totalScale;    // 当前缩放比例
@property (nonatomic,assign) CGFloat normalX;       // 未进行缩放移动状态下的origin.x
@property (nonatomic,assign) CGFloat normalY;       // 未进行缩放移动状态下的origin.y
@property (nonatomic,assign) CGFloat normalWidth;   // 未进行缩放移动状态下的size.width
@property (nonatomic,assign) CGFloat normalHeight;  // 未进行缩放移动状态下的size.height

@property (nonatomic,strong) UIPinchGestureRecognizer *pinchGesture;    // 双指缩放手势
@property (nonatomic,strong) UIPanGestureRecognizer *panGesture;        // 单指移动手势
@end

@implementation UIView (Zoomable)
#pragma mark - Init
+ (instancetype)allocWithZone:(struct _NSZone *)zone{
    UIView *instance = [super allocWithZone:zone];
    if(instance){
        instance.zoomable       = NO;
        instance.scaleMax       = 4.0;
        instance.scaleMin       = 1.0;
        instance.totalScale     = 1.0;
    }
    return instance;
}

#pragma mark - Public
- (void)zoomRestore{
    // 由于第一次触发手势才记录x/y/w/h信息,所以还原时需要判断一下原始信息的可用性,避免没有进行触发手势时直接还原导致的异常
    if(self.normalHeight == 0 && self.normalWidth == 0){
        return;
    }
    
    self.transform  = CGAffineTransformMakeScale(1, 1);
    self.frame      = CGRectMake(self.normalX, self.normalY, self.normalWidth, self.normalHeight);
    
    self.totalScale     = 1.0;
    self.normalHeight   = 0;
    self.normalWidth    = 0;
}

#pragma mark - Property
- (void)setZoomable:(BOOL)zoomable{
    id obj = objc_getAssociatedObject(self, kZoomable);
    if(nil != obj && [obj boolValue] == zoomable){
        return;
    }
    objc_setAssociatedObject(self, kZoomable, @(zoomable), OBJC_ASSOCIATION_RETAIN_NONATOMIC);
    zoomable ? [self installRecognizer] : [self uninstallRecognizer];
}
- (BOOL)zoomable{
    return [objc_getAssociatedObject(self, kZoomable) boolValue];
}
- (void)setScaleMax:(CGFloat)scaleMax{
    objc_setAssociatedObject(self, kScaleMax, @(scaleMax), OBJC_ASSOCIATION_RETAIN_NONATOMIC);
}
-(CGFloat)scaleMax{
    return [objc_getAssociatedObject(self, kScaleMax) floatValue];
}
- (void)setScaleMin:(CGFloat)scaleMin{
    objc_setAssociatedObject(self, kScaleMin, @(scaleMin), OBJC_ASSOCIATION_RETAIN_NONATOMIC);
}
-(CGFloat)scaleMin{
    return [objc_getAssociatedObject(self, kScaleMin) floatValue];
}
- (void)setTotalScale:(CGFloat)totelScale{
    objc_setAssociatedObject(self, kTotalScale, @(totelScale), OBJC_ASSOCIATION_RETAIN_NONATOMIC);
}
-(CGFloat)totalScale{
    return [objc_getAssociatedObject(self, kTotalScale) floatValue];
}

#pragma mark - Property
- (void)setNormalX:(CGFloat)normalX{
    objc_setAssociatedObject(self, kNormalX, @(normalX), OBJC_ASSOCIATION_RETAIN_NONATOMIC);
}
- (CGFloat)normalX{
    return [objc_getAssociatedObject(self, kNormalX) floatValue];
}
- (void)setNormalY:(CGFloat)normalY{
    objc_setAssociatedObject(self, kNormalY, @(normalY), OBJC_ASSOCIATION_RETAIN_NONATOMIC);
}
- (CGFloat)normalY{
    return [objc_getAssociatedObject(self, kNormalY) floatValue];
}
- (void)setNormalWidth:(CGFloat)normalWidth{
    objc_setAssociatedObject(self, kNormalWidth, @(normalWidth), OBJC_ASSOCIATION_RETAIN_NONATOMIC);
}
- (CGFloat)normalWidth{
    return [objc_getAssociatedObject(self, kNormalWidth) floatValue];
}
- (void)setNormalHeight:(CGFloat)normalHeight{
    objc_setAssociatedObject(self, kNormalHeight, @(normalHeight), OBJC_ASSOCIATION_RETAIN_NONATOMIC);
}
- (CGFloat)normalHeight{
    return [objc_getAssociatedObject(self, kNormalHeight) floatValue];
}

#pragma mark - Property
- (void)setPanGesture:(UIPanGestureRecognizer *)panGesture{
    objc_setAssociatedObject(self, kPanGesture, panGesture, OBJC_ASSOCIATION_RETAIN_NONATOMIC);
}
- (UIPanGestureRecognizer *)panGesture{
    return objc_getAssociatedObject(self, kPanGesture);
}
- (void)setPinchGesture:(UIPinchGestureRecognizer *)pinchGesture{
    objc_setAssociatedObject(self, kPinchGesture, pinchGesture, OBJC_ASSOCIATION_RETAIN_NONATOMIC);
}
- (UIPinchGestureRecognizer *)pinchGesture{
    return objc_getAssociatedObject(self, kPinchGesture);
}

#pragma mark - 手势相关
- (void) saveRestoreRect{
    if(self.normalWidth == 0 && self.normalHeight == 0){
        CGRect frame        = self.frame;
        self.normalWidth    = frame.size.width;
        self.normalHeight   = frame.size.height;
        self.normalX        = frame.origin.x;
        self.normalY        = frame.origin.y;
    }
}
- (void) installRecognizer{
    if(nil == self.pinchGesture) self.pinchGesture = [[UIPinchGestureRecognizer alloc] initWithTarget:self action:@selector(onPinchEvent:)];
    if(nil == self.panGesture)   self.panGesture = [[UIPanGestureRecognizer alloc] initWithTarget:self action:@selector(onPanEvent:)];

    [self addGestureRecognizer:self.pinchGesture];
    [self addGestureRecognizer:self.panGesture];
}
- (void) uninstallRecognizer{
    if(nil != self.pinchGesture) [self removeGestureRecognizer:self.pinchGesture];
    if(nil != self.panGesture)   [self removeGestureRecognizer:self.panGesture];
}

- (void)onPinchEvent:(UIPinchGestureRecognizer *)pinchGestureRecognizer{
    UIView *view    = pinchGestureRecognizer.view;
    CGFloat scale   = pinchGestureRecognizer.scale;
    if((scale>1&&self.totalScale >= self.scaleMax) || (scale<1&&self.totalScale < self.scaleMin)){
        return;
    }

    if(pinchGestureRecognizer.state == UIGestureRecognizerStateBegan){
        [self saveRestoreRect];
    }
    else if (pinchGestureRecognizer.state == UIGestureRecognizerStateChanged) {
        self.totalScale *= scale;
        if(self.totalScale > self.scaleMax) self.totalScale = self.scaleMax;
        if(self.totalScale < self.scaleMin) self.totalScale = self.scaleMin;

        CGPoint point   = view.frame.origin;
        CGSize size     = view.frame.size;
        CGPoint center  = view.center;
        if(point.x>self.normalX){
            center.x -= point.x-self.normalX;
        }
        else{
            CGFloat maxX = size.width + point.x;
            if(maxX< self.normalX+self.normalWidth){
                center.x += self.normalX+self.normalWidth - maxX;
            }
        }
        if(point.y>self.normalY){
            center.y -= point.y-self.normalY;
        }
        else{
            CGFloat maxY = size.height + point.y;
            if(maxY < self.normalY+self.normalHeight){
                center.y += self.normalY+self.normalHeight - maxY;
            }
        }
        pinchGestureRecognizer.scale = 1;   // 重置当前比例

        [UIView beginAnimations:nil context:nil];
        [UIView setAnimationDuration:0.4];
        view.transform = CGAffineTransformMakeScale(self.totalScale, self.totalScale);
        view.center = center;
        [UIView commitAnimations];
    }
    else{
        
    }
}
- (void)onPanEvent:(UIPanGestureRecognizer *)panGestureRecognizer{
    UIView *view = panGestureRecognizer.view;
    
    if(panGestureRecognizer.state == UIGestureRecognizerStateBegan){
        [self saveRestoreRect];
    }
    else if (panGestureRecognizer.state == UIGestureRecognizerStateChanged) {
        CGPoint translation = [panGestureRecognizer translationInView:view.superview];
        CGPoint center      = (CGPoint){view.center.x + translation.x, view.center.y + translation.y};
        CGFloat halfOfW     = view.frame.size.width * 0.5;
        CGFloat halfOfH     = view.frame.size.height * 0.5;

        if(center.x > halfOfW+self.normalX){
            center.x = halfOfW+self.normalX;
        }
        else{
            CGFloat maxX = center.x + halfOfW;
            if(maxX < self.normalX+self.normalWidth){
                center.x += self.normalX+self.normalWidth - maxX;
            }
        }

        if(center.y > halfOfH+self.normalY){
            center.y = halfOfH+self.normalY;
        }
        else{
            CGFloat maxY = center.y + halfOfH;
            if(maxY < self.normalY+self.normalHeight){
                center.y += self.normalY+self.normalHeight - maxY;
            }
        }
        [panGestureRecognizer setTranslation:CGPointZero inView:view.superview];
        
        [UIView beginAnimations:nil context:nil];
        [UIView setAnimationDuration:0.2];
        [view setCenter:center];
        [UIView commitAnimations];
    }
    else{
        
    }
}
@end
