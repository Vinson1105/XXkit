#import "XXlistViewShell.h"
#import "../XXocDefine.h"

#define ROW_HEIGHT  30          // 默认预测高度
#define ROW_TITLE   @"_Title_"  // cell的主标题

@interface XXlistViewShell()
/** 组件所控制的目标控件 */
@property (weak,nonatomic) UITableView *tableView;
@end

@implementation XXlistViewShell

#pragma mark - 构建和析构
// 初始化列表组件控制目标
- (void) initContext{
    _tableView              = self.context;
    
    _tableView.delegate     = self;
    _tableView.dataSource   = self;
    
    _tableView.estimatedRowHeight           = 30;           // 预测行高
    _tableView.estimatedSectionHeaderHeight = 0;            // 预测页头高度
    _tableView.estimatedSectionFooterHeight = 0;            // 预测页脚高度
    _tableView.rowHeight = UITableViewAutomaticDimension;   // 自适应cell的高度
    
    switch (self.cellLoadType) {
        case XXcellLoadType_Default:
            break;
        case XXcellLoadType_Class:
            [_tableView registerClass:NSClassFromString(self.cellType) forCellReuseIdentifier:self.cellType];
            break;
        case XXcellLoadType_Nib:
            [_tableView registerNib:[UINib nibWithNibName:self.cellType bundle:nil] forCellReuseIdentifier:self.cellType];
            break;
        default:
            break;
    }
}

#pragma mark - 公开接口: <设置固定行高>
// 设置固定行高
- (void) setRowHeight:(NSUInteger)rowHeight{
    if (nil != [self tableView]) {
        [[self tableView] setRowHeight:rowHeight];
    }
}

#pragma mark - 代理: <UITableViewDelegate>
// 页头高度
- (CGFloat)tableView:(UITableView *)tableView heightForHeaderInSection:(NSInteger)section{
    return 0.01;
}
// 页脚高度
- (CGFloat)tableView:(UITableView *)tableView heightForFooterInSection:(NSInteger)section{
    return 0.01;
}
// 行选择
- (NSIndexPath *)tableView:(UITableView *)tableView willSelectRowAtIndexPath:(NSIndexPath *)indexPath{
    if (nil != self.onRowTouchEvent) {
        self.onRowTouchEvent(indexPath.row, [self.data objectAtIndex:indexPath.row]);
    }
    return nil;
}

#pragma mark - 代理: <UITableViewDataSource>
// tableViewCell的重用以及构建
- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath{
    
    // [1] 取出已经生成的cell
    NSString *cellType = self.cellType;
    UITableViewCell *cell = nil;
    if (XXcellLoadType_Default == self.cellLoadType) {
        cell = [_tableView dequeueReusableCellWithIdentifier:cellType];
    }
    else{
        cell = [_tableView dequeueReusableCellWithIdentifier:cellType forIndexPath:indexPath];  
    }
    
    // [2] 无可用cell,重新分配
    if(nil == cell){
        if (XXcellLoadType_Default == self.cellLoadType) {
            // 创建默认
            cell = [[UITableViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:cellType];
        }
        else{
            // 创建自定义cell
            Class rowClass  = NSClassFromString(cellType);
            cell            = [[rowClass alloc] initWithReuseIdentifier:cellType];
        }
    }
    if (nil == cell) {
        XXNSLog(@"CellType=%@, 没有找对应的自定义cell类型", cellType);
        return nil;
    }
    
    // [3] 取出row对应的数据
    id data = [self.data objectAtIndex:indexPath.row];
    if (XXcellLoadType_Default == self.cellLoadType) {
        // [4] 使用默认cell时,需要判断数据是字典还是字符串,如果是字符串,直接视为text,否则需要按约定的键值设置
        if ([data isKindOfClass:NSClassFromString(@"NSString")]) {
            cell.textLabel.text = data;
        }
        else{
            NSEnumerator *keyEnumerator = [data keyEnumerator];
            NSString *key;
            while (key = [keyEnumerator nextObject]) {
                if ([key isEqualToString:ROW_TITLE]) {
                    cell.textLabel.text = [data objectForKey:ROW_TITLE];
                }
                else{
                    
                }
            }
        }
    }
    else{
        // [5] 使用自定义cell,需要判断是否遵循了协议,并进行对应设置
        if(![cell conformsToProtocol:@protocol(XXcellProtocol)]){
            XXNSLog(@"CellType=%@, 没有遵循XXcellProtocol", cellType);
        }
        else{
            UITableViewCell<XXcellProtocol> *xcell = (UITableViewCell<XXcellProtocol>*)cell;
            
            // cell的位置/数据设置
            [xcell setIndexPath:indexPath Data:data];
            
            // cell的事件处理block设置
            if (nil != self.onCellEvent) {
                xcell.onEvent = self.onCellEvent;
            }
        }
    }
    return cell;
}
// 节点数量
- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView{
    return 1;
}
// 节点中行数量
- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section{
    return nil == self.data ? 0 : self.data.count;
}
@end
