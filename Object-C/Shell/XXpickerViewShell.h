/**
 2020.07.21
 1、使用默认row显示的简单封装
 */

#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN

@interface XXpickerViewShell : NSObject
@property (nonatomic,copy) void(^onSelectChanged)(XXpickerViewShell *shell, NSArray *selecteds);

/** 目标UIPickerView */
@property (nonatomic,strong,readonly) UIPickerView *pickerView;
/** PickerView中行高，默认：30 */
@property (nonatomic,assign) int rowHeight;
/** PickerView中unselected的字体颜色，默认gray */
@property (nonatomic,strong) UIColor *unselectedColor;
/** PickerView中selected的字体颜色，默认blue */
@property (nonatomic,strong) UIColor *selectedColor;
/** 可选项数据 ，其中componentData.count就等同component的数量*/
@property (nonatomic,strong) NSArray<NSArray*> *componentData;

/**
 设置目标UIPickerView
 */
- (void)shell:(UIPickerView*)pickerView;

/**
 设置某一个字符串对应row为当前选中
 @param string 在指定的component中的某一个数据
 @param component 对应的component编号
 */
- (void)selectString:(NSString*)string atComponent:(int)component;

/**
 设置每一个component的选中
 */
- (void)selectString:(NSArray<NSString*>*)string;
@end

NS_ASSUME_NONNULL_END
