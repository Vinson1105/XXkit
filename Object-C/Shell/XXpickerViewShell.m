#import "XXpickerViewShell.h"

@interface XXpickerViewShell()<UIPickerViewDelegate,UIPickerViewDataSource>
@property (nonatomic,strong) NSMutableDictionary<NSNumber*,NSNumber*> *componentToSelected;
@end

@implementation XXpickerViewShell
#pragma mark - <初始化>
- (instancetype)init{
    self = [super init];
    if(self){
        _componentData = [NSArray new];
        _rowHeight = 30;
        _unselectedColor = UIColor.grayColor;
        _selectedColor = UIColor.blueColor;
        _componentToSelected = [NSMutableDictionary new];
    }
    return self;
}

#pragma mark - <Public>
- (void)shell:(UIPickerView*)pickerView{
    if(_pickerView){
        return;
    }
    _pickerView = pickerView;
    _pickerView.dataSource = self;
    _pickerView.delegate = self;
}
- (void)setComponentData:(NSArray <NSArray*>*)data{
    [self resetSelectedIndex];
    _componentData = data;
    [_pickerView reloadAllComponents];
}
- (void)selectString:(NSString*)string atComponent:(int)component{
    if(nil==_componentData){
        return;
    }
    
    if(component>=_componentData.count){
        return;
    }
    
    NSUInteger index= [_componentData[component] indexOfObject:string];
    if(index == NSNotFound){
        return;
    }
    
    NSNumber *selected = [_componentToSelected objectForKey:@(component)];
    if(nil!=selected && index==[selected intValue]){
        return;
    }
    
    [_pickerView selectRow:index inComponent:component animated:YES];
}
- (void)selectString:(NSArray<NSString*>*)string{
    for (int component=0; component<string.count; component++) {
        [self selectString:string[component] atComponent:component];
    }
}

#pragma mark - <UIPickerViewDelegate>
- (NSString *)pickerView:(UIPickerView *)pickerView titleForRow:(NSInteger)row forComponent:(NSInteger)component{
        return _componentData[component][row];
}
- (void)pickerView:(UIPickerView *)pickerView didSelectRow:(NSInteger)row inComponent:(NSInteger)component{
    [_componentToSelected setObject:@(row) forKey:@(component)];
    [pickerView reloadAllComponents];
}
- (CGFloat)pickerView:(UIPickerView *)pickerView rowHeightForComponent:(NSInteger)component{
    return _rowHeight;
}
- (NSAttributedString *)pickerView:(UIPickerView *)pickerView attributedTitleForRow:(NSInteger)row forComponent:(NSInteger)component{
    UIColor *color = [_componentToSelected[@(component)] intValue] == row? _selectedColor : _unselectedColor;
    NSAttributedString *string = [[NSAttributedString alloc] initWithString:_componentData[component][row] attributes:@{NSForegroundColorAttributeName:color}];
    return string;
}

#pragma mark - <UIPickerViewDataSource>
- (NSInteger)numberOfComponentsInPickerView:(UIPickerView *)pickerView{
    return nil==_componentData?0:_componentData.count;
}
- (NSInteger)pickerView:(UIPickerView *)pickerView numberOfRowsInComponent:(NSInteger)component{
    return [_componentData[component] count];
}

#pragma mark - <Private>
- (void)resetSelectedIndex{
    NSEnumerator *keyEnumer = _componentToSelected.keyEnumerator;
    NSNumber *component = nil;
    while (nil != (component=keyEnumer.nextObject)) {
        _componentToSelected[component] = @(0);
    }
}
@end
