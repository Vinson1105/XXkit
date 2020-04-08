#import "XXtableViewShell.h"

#define kReuseRowDefault @"ReuseRowDefault"
#define kReuseHeaderDefault @"ReuseHeaderDefault"
#define kReuseFooterDefault @"ReuseFooterDefault"

#define kHeader @"Header"
#define kRow @"Row"
#define kFooter @"Footer"

@interface XXtableViewShell()
@property (nonatomic,strong) NSMutableArray *sectionDatas;
@end

@implementation XXtableViewShell
// MARK: 初始化
- (instancetype)init{
    self = [super init];
    if (self) {
        _sectionDatas = [NSMutableArray new];
    }
    return self;
}

// MARK: public
- (void)shell:(UITableView*)target{
    if(nil != _target) return;
    _target = target;
    _target.delegate = self;
    _target.dataSource = self;
}
- (void)configSectionWithHeaders:(nullable NSArray*)headers rows:(NSArray*)rows footers:(nullable NSArray*)footers{
    
    int sectionCount = (int)rows.count;
    for (int index = 0; index < sectionCount; index++) {
        NSMutableDictionary *section = [NSMutableDictionary new];
        if(nil != headers) [section setObject:headers[index] forKey:kHeader];
        if(nil != footers) [section setObject:footers[index] forKey:kFooter];
        [section setObject:rows[index] forKey:kRow];
        [_sectionDatas addObject:section];
    }
        
    [_target reloadData];
}
- (void)configSectionWithHeader:(nullable id)header row:(nullable NSArray*)row footer:(nullable id)footer{
    NSMutableDictionary *section = [NSMutableDictionary new];
    if(nil != header) [section setObject:header forKey:kHeader];
    if(nil != footer) [section setObject:footer forKey:kFooter];
    if(nil != row) [section setObject:row forKey:kRow];
    [_sectionDatas addObject:section];
}
- (void)configFinished{
    [_target reloadData];
}

// MARK: public
- (void)addSectionWithHeader:(nullable id)header row:(nullable NSArray*)row footer:(nullable id)footer{
    NSMutableDictionary *section = [NSMutableDictionary new];
    if(nil != header) [section setObject:header forKey:kHeader];
    if(nil != footer) [section setObject:footer forKey:kFooter];
    if(nil != row) [section setObject:row forKey:kRow];
    [_sectionDatas addObject:section];
    [_target reloadData];
}
- (void)insertSectionWithHeader:(nullable id)header row:(nullable NSArray*)row footer:(id)footer atIndex:(int)index{
    NSMutableDictionary *section = [NSMutableDictionary new];
    if(nil != header) [section setObject:header forKey:kHeader];
    if(nil != footer) [section setObject:footer forKey:kFooter];
    if(nil != row) [section setObject:row forKey:kRow];
    [_sectionDatas insertObject:section atIndex:index];
    [_target reloadData];
}
- (void)removeSectionAtIndex:(int)index{
    [_sectionDatas removeObjectAtIndex:index];
    [_target reloadData];
}
- (void)resetRow:(nullable NSArray*)row atSection:(int)section{
    NSMutableDictionary *sectionData = [_sectionDatas objectAtIndex:section];
    if(nil == sectionData){
        return;
    }
    if(nil == row){
        [sectionData removeObjectForKey:kRow];
    }
    else{
        [sectionData setObject:row forKey:kRow];
    }
    [_target reloadData];
}

// MARK: 
@end

