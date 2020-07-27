#import "XXpickerViewShell.h"

@interface XXpickerViewShell()<UIPickerViewDelegate,UIPickerViewDataSource>
@property (nonatomic,strong) NSMutableArray *componentData;
@property (nonatomic,assign) NSInteger lastSelected;
@end

@implementation XXpickerViewShell
#pragma mark - <初始化>
- (instancetype)init{
    self = [super init];
    if(self){
        _componentData = [NSMutableArray new];
        _rowHeight = 30;
        _unselectedColor = UIColor.grayColor;
        _selectedColor = UIColor.blueColor;
    }
    return self;
}

#pragma mark - <配置>
- (void)shell:(UIPickerView*)pickerView{
    if(_pickerView){
        return;
    }
    _pickerView = pickerView;
    _pickerView.dataSource = self;
    _pickerView.delegate = self;
}
- (void)setData:(NSArray*)data atComponent:(int)component{
    _lastSelected = 0;
    if(component<_componentData.count){
        _componentData[component] = data;
        [_pickerView selectRow:0 inComponent:component animated:NO];
    }
    else if(component==_componentData.count){
        [_componentData addObject:data];
    }
    else{
        
    }
    [_pickerView reloadComponent:component];
}
- (void)removeDataAtComponent:(int)component{
    if(component >= _componentData.count){
        return;
    }
    [_componentData removeObjectAtIndex:component];
}
- (void)selectRow:(int)row atComponent:(int)component{
    [_pickerView selectRow:row inComponent:component animated:YES];
}

#pragma mark - <UIPickerViewDelegate>
- (NSString *)pickerView:(UIPickerView *)pickerView titleForRow:(NSInteger)row forComponent:(NSInteger)component{
        return _componentData[component][row];
}
- (void)pickerView:(UIPickerView *)pickerView didSelectRow:(NSInteger)row inComponent:(NSInteger)component{
    _lastSelected = row;
    [pickerView reloadAllComponents];
}
- (CGFloat)pickerView:(UIPickerView *)pickerView rowHeightForComponent:(NSInteger)component{
    return _rowHeight;
}
- (NSAttributedString *)pickerView:(UIPickerView *)pickerView attributedTitleForRow:(NSInteger)row forComponent:(NSInteger)component{
    NSAttributedString *string = nsar
}

#pragma mark - <UIPickerViewDataSource>
- (NSInteger)numberOfComponentsInPickerView:(UIPickerView *)pickerView{
    return _componentData.count;
}
- (NSInteger)pickerView:(UIPickerView *)pickerView numberOfRowsInComponent:(NSInteger)component{
    return [_componentData[component] count];
}
@end
