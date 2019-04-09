

#import "XXshellCellBase.h"
#import "../XXocDefine.h"
#define DEFAULT_CELL_TYPE @"_DefaultCell_"    // 默认cell类型

@interface XXshell(){
    
}
@property (nonatomic,strong,nullable) NSMutableArray *xXdata;
@end

@implementation XXshell
// 初始化为使用默认cell
- (instancetype) initWithContext:(id)context;{
    self = [super init];
    if (self) {
        _context        = context;
        _cellType       = DEFAULT_CELL_TYPE;
        _cellLoadType   = XXcellLoadType_Default;
        
        [self initContext];
    }
    return self;
}
// 初始化为使用手动布局cell
- (instancetype) initWithContext:(id)context ClassCellType:(NSString*)cellType{
    self = [super init];
    if (self) {
        _context        = context;
        _cellType       = cellType;
        _cellLoadType   = XXcellLoadType_Class;
        
        [self initContext];
    }
    return self;
}
// 初始化为使用xib布局cell
- (instancetype) initWithContext:(id)context NIbCellType:(NSString *)cellType{
    self = [super init];
    if (self) {
        _context        = context;
        _cellType       = cellType;
        _cellLoadType   = XXcellLoadType_Nib;
        
        [self initContext];
    }
    return self;
}
- (void) initContext{
    // 这里使用这一些控件的初始化
}
@end

