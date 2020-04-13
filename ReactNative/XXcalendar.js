/**
 * 通用日历显示
 * 
 * 2020-04-11
 * 1、新增weekTitle和date属性，其中date是JS的Date类型
 * 2、onPress的形参从（year，month，day）更改为（date）
 * 3、FIXME:星期的标题和日期可能对不上，或者不是垂直居中，需要调整一下计算方式
 * 
 * 创建于2020-04-10
 * 定义以下属性
 * prevMonthImageSource     // 上一个月按键的图片
 * nextMonthImageSource     // 下一个月按键的图片
 * weekTitle                // [日,一,二,三,四,五,六]
 * date                     // Date()
 * 
 * titleBarTextStyle        
 *  color
 *  fontSize
 * titleBarButtonStyle
 *  width
 *  height
 * titleBarBackgroundStyle
 *  height
 *  backgroundColor
 * 
 * weekBarTextStyle
 *  color                   // 字体颜色
 *  fontSize                // 字体大小
 * weekBarBackgroundStyle
 *  height                  // bar高度
 *  background              // bar背景颜色
 * 
 * dayBarStyle
 *  backgroundColor         // bar背景颜色
 * 
 * dayBarItemTextStyle          // item的通用样式
 *  color
 *  fontSize
 * dayBarItemBackgroundStyle
 *  borderColor
 *  borderWidth
 *  backgroundColor
 * 
 * dayBarItemTextNormalStyle            // 普通日期样式
 * dayBarItemBackgroundNormalStyle       
 * 
 * dayBarItemTextSelectedStyle          // 选中的日期样式
 * dayBarItemBackgroundSelectedStyle
 * 
 * dayBarItemTextDisabledStyle          // 非本月的日期样式
 * dayBarItemBackgroundDisabledStyle
 * 
 * dayBarItemTextTodayStyle             // 今天的日期样式
 * dayBarItemBackgroundDisabledStyle
 * 
 * onPress({year:2020,month:10,day:10})
 */

import React, {Component} from 'react';
import {View, Text, StyleSheet, FlatList} from 'react-native';
import XXbutton from './XXbutton'

const _today = new Date();

export default class XXcalendar extends Component{
    static defaultProps = {
        weekTitle: ['日','一','二','三','四','五','六'],
        date: new Date,
        //weekTitle: ['SUN','MON','TUS','WED','THU','FIR','SAT'],
    } 

    constructor(props){
        super(props);

        //let today = new Date();
        this.state = {
            _yearSelected: this.props.date.getFullYear(),
            _monthSelected: this.props.date.getMonth()+1,
            _daySelected: this.props.date.getDate(),

            _yearShowing: this.props.date.getFullYear(),
            _monthShowing: this.props.date.getMonth()+1,

            _width: this.props.style.width,
            _height: this.props.style.height,
            _cellSpace: 6,
            _cellColumn: 7,
            _cellSize: (this.props.style.width - 6*8) / 7,
        }
    }
    
    render() {
        return(
            <View style={[
                {
                    flexDirection:'column'
                },
                this.props.style
                ]}>
                {this._renderTopBar()}
                {this._renderWeekBar()}
                {this._renderDayBar()}
            </View>
        );
    }

    _renderTopBar() {
        const titleBarBackgroundStyle = this.props.titleBarStyle;
        const titleBarTextStyle = this.props.titleBarTextStyle;
        const titleBarButtonStyle = this.props.titleBarButtonStyle;
        return (
            <View style={[
                styles.titleBarBackground,
                titleBarBackgroundStyle]}>
            
                <XXbutton 
                    type={'image'} 
                    source={this.props.prevMonthImageSource} 
                    style={[
                        styles.titleBarButton, 
                        titleBarButtonStyle,
                    ]}
                    onPress={_=>{
                        const month = 1 === this.state._monthShowing ? 12 : this.state._monthShowing-1;
                        const year  = 1 === this.state._monthShowing ? this.state._yearShowing-1 : this.state._yearShowing;
                        this.setState({
                            _yearShowing: year,
                            _monthShowing: month,
                        })
                    }}
                />
                <Text style={[styles.titleBarText,titleBarTextStyle]}>{this.state._yearShowing+'/'+this.state._monthShowing}</Text>
                <XXbutton 
                    type={'image'} 
                    source={this.props.nextMonthImageSource} 
                    style={[
                        styles.titleBarButton, 
                        titleBarButtonStyle,
                    ]}
                    onPress={_=>{
                        const month = 12 === this.state._monthShowing ? 1 : this.state._monthShowing+1;
                        const year  = 12 === this.state._monthShowing ? this.state._yearShowing+1 : this.state._yearShowing;
                        this.setState({
                            _yearShowing: year,
                            _monthShowing: month,
                        })
                    }}
                />
            </View>
        );
    }

    _renderWeekBar() {
        const weekBarBackgroundStyle = this.props.weekBarBackgroundStyle;
        const weekBarTextStyle = this.props.weekBarTextStyle;
        //flexDirection:'row', height:40, width:'100%', justifyContent:'space-around', backgroundColor:'green', alignItems:'center',
        return (
            <View style={[styles.weekBarBackground, {paddingHorizontal:this.state._cellSpace/2}, weekBarBackgroundStyle]}>
                <Text style={[styles.weekBarText, weekBarTextStyle]}>{this.props.weekTitle[0]}</Text>
                <Text style={[styles.weekBarText, weekBarTextStyle]}>{this.props.weekTitle[1]}</Text>
                <Text style={[styles.weekBarText, weekBarTextStyle]}>{this.props.weekTitle[2]}</Text>
                <Text style={[styles.weekBarText, weekBarTextStyle]}>{this.props.weekTitle[3]}</Text>
                <Text style={[styles.weekBarText, weekBarTextStyle]}>{this.props.weekTitle[4]}</Text>
                <Text style={[styles.weekBarText, weekBarTextStyle]}>{this.props.weekTitle[5]}</Text>
                <Text style={[styles.weekBarText, weekBarTextStyle]}>{this.props.weekTitle[6]}</Text>
            </View>
        );
    }
    _renderDayBar() {
        const dayBarStyle = this.props.dayBarStyle;
        let days = this._getDays(this.state._yearShowing, this.state._monthShowing, true);
        return(
            <FlatList
                style={[styles.dayBar,{borderBottomLeftRadius:this.props.style.borderRadius, borderBottomRightRadius:this.props.style.borderRadius}, dayBarStyle]}
                renderItem={({index,item}) => this._renderDayItem(index,item)}
                data={days}
                numColumns={this.state._cellColumn}
                keyExtractor={(item ,index)=>{return "index"+index+item;}}
            />
        );
    }
    _renderDayItem(index,item){
        const backgroundStyle = {
            width:this.state._cellSize, 
            height:this.state._cellSize, 
            marginLeft:this.state._cellSpace, 
            marginTop:this.state._cellSpace,
            borderWidth: 1.5,
            borderRadius: this.state._cellSize / 2,
        };

        let textStyleOfCustom, textStyleOfSystem;
        let bgStyleOfCustom, bgStyleOfSystem;
        if(item.isGreedy){
            textStyleOfCustom   = this.props.dayBarItemTextDisabled;
            textStyleOfSystem   = styles.dayBarItemTextDisabled;
            bgStyleOfCustom     = this.props.dayBarItemBackgroundDisabled;
            bgStyleOfSystem     = styles.dayBarItemBackgroundDisabled;
        }
        else if(item.isSelected){
            textStyleOfCustom   = this.props.dayBarItemTextSelected;
            textStyleOfSystem   = styles.dayBarItemTextSelected;
            bgStyleOfCustom     = this.props.dayBarItemBackgroundSelected;
            bgStyleOfSystem     = styles.dayBarItemBackgroundSelected;
        }
        else if(item.isToday){
            textStyleOfCustom   = this.props.dayBarItemTextToday;
            textStyleOfSystem   = styles.dayBarItemTextToday;
            bgStyleOfCustom     = this.props.dayBarItemBackgroundToday;
            bgStyleOfSystem     = styles.dayBarItemBackgroundToday;
        }
        else{
            textStyleOfCustom   = this.props.dayBarItemTextNormal;
            textStyleOfSystem   = styles.dayBarItemTextNormal;
            bgStyleOfCustom     = this.props.dayBarItemBackgroundNormal;
            bgStyleOfSystem     = styles.dayBarItemBackgroundNormal;
        }

        return (
            <XXbutton
                style={[backgroundStyle, this.props.dayBarItemBackground, bgStyleOfSystem, bgStyleOfCustom]}
                title={item.day}
                titleStyle={[styles.dayBarItemText, this.props.dayBarItemText, textStyleOfSystem, textStyleOfCustom]}
                onPress={_=>{
                    const year = item.year;
                    const month = item.month;
                    const day = item.day;
                    if(year===this.state._yearShowing && month===this.state._monthSelected && day===this.state._daySelected){
                        return;
                    }

                    this.setState({
                        _yearSelected: year,
                        _monthSelected: month,
                        _daySelected: day,
                    })
                    if(this.props.onPress){
                        this.props.onPress(new Date(year,month-1,day));
                    }
                }}
            />
        )
    }

    _getDays(year,month,greedy){
        /**
         * 返回指定月份的所有日期
         * 
         * {
         *      year:
         *      month:
         *      day:
         *      week:
         *      isToday:
         *      isSelected:
         *      [isGreedy:]
         * }
         */

        let today = new Date;
        const yearOfToday   = today.getFullYear();
        const monthOfToday  = today.getMonth()+1;
        const dayOfToday    = today.getDate();
        const weekOfToday   = today.getDay();    // 0:星期日 6:星期六

        let firstOfMonth    = new Date(year,month-1,1); // 该月的第一天
        let lastOfMonth     = new Date(year,month,0);   // 该月的最后一天

        let days = [];
        if(greedy){
            let count = firstOfMonth.getDay();
            let yearOfPrevMonth = 1===month ? year-1 : year;
            let monthOfPrevMonth = 1===month ? 12 : month-1;

            let lastOfPrevMonth = new Date(yearOfPrevMonth, monthOfPrevMonth, 0);
            const maxDate = lastOfPrevMonth.getDate();
            for(let index = maxDate-count+1; index<=maxDate; index++){
                let info = [];
                info['year'] = yearOfPrevMonth;
                info['month'] = monthOfPrevMonth;
                info['day'] = index;
                info['isToday'] = yearOfToday===yearOfPrevMonth&&monthOfToday===monthOfPrevMonth&&dayOfToday===index;
                info['isSelected'] = this.state._yearSelected===yearOfPrevMonth&&this.state._monthSelected===monthOfPrevMonth&&this.state._daySelected===index;
                info['isGreedy'] = true;
                days.push(info);
            }
        }

        {
            let count = lastOfMonth.getDate();
            for(let index = 1; index <= count; index++){
                //let day = new Date(year,month-1,index);
                let info = [];
                info['year'] = year;
                info['month'] = month;
                info['day'] = index;
                //info['week'] = day.getDay();
                info['isToday'] = yearOfToday===year&&monthOfToday===month&&dayOfToday===index;
                info['isSelected'] = this.state._yearSelected===year&&this.state._monthSelected===month&&this.state._daySelected===index;
                info['isGreedy'] = false;
                days.push(info);
            }
        }

        if(greedy){
            let count = 6 - lastOfMonth.getDay();

            let yearOfNextMonth = 12===month ? year+1 : year;
            let monthOfNextMonth = 12===month ? 1 : month+1;

            for(let index = 1; index <= count; index++){
                let info = [];
                info['year'] = yearOfNextMonth;
                info['month'] = monthOfNextMonth;
                info['day'] = index;
                info['isToday'] = yearOfToday===yearOfNextMonth&&monthOfToday===monthOfNextMonth&&dayOfToday===index;
                info['isSelected'] = this.state._yearSelected===yearOfNextMonth&&this.state._monthSelected===monthOfNextMonth&&this.state._daySelected===index;
                info['isGreedy'] = true;
                days.push(info);
            }
        }
        return days;
    }
}

const styles = StyleSheet.create({
    // 标题bar的样式
    titleBarBackground: {
        width:'100%',
        height:45,
        backgroundColor:'transparent',
        flexDirection:'row',
    },
    titleBarButton: {
        marginHorizontal:10, 
        width: 16,
        height: 16,
        alignSelf: 'center',
    },
    titleBarText: {
        color: 'black',
        fontSize: 12,
        flex: 1,
        textAlign: 'center',
        alignSelf: 'center',
    },

    // 星期bar的样式
    weekBarBackground:{
        backgroundColor: 'transparent',
        height: 30,
        width: '100%',
        flexDirection:'row',
        justifyContent:'space-around',
        alignItems:'center',
    },
    weekBarText:{
        fontSize: 14,
        color: '#333333'
    },

    // 日期bar的背景
    dayBar: {
        flex:1, 
        alignSelf:'stretch',
        backgroundColor: 'transparent'
    },

    /** 注意：一个完整的item样式 = 通用样式 + 状态样式 */
    //（通用）每一个日期的背景 和 每一个日期的字体
    dayBarItemText:{
        fontSize: 12,
    },

    //（普通）每一个日期的背景 和 每一个日期的字体
    dayBarItemBackgroundNormal:{
        borderColor: 'transparent',
        backgroundColor: 'transparent',
    },
    dayBarItemTextNormal:{
        color: '#333333',
    },

    //（选中）每一个日期的背景 和 每一个日期的字体
    dayBarItemBackgroundSelected:{
        borderColor: '#41B18B',
        backgroundColor: '#41B18B',
    },
    dayBarItemTextSelected:{
        color: 'white',
    },

    //（今天）每一个日期的背景 和 每一个日期的字体
    dayBarItemBackgroundToday:{
        borderColor: '#41B18B',
        backgroundColor: 'transparent',
    },
    dayBarItemTextToday:{
        color: '#41B18B',
    },

    //（不可用、非本月）每一个日期的背景 和 每一个日期的字体
    dayBarItemBackgroundDisabled:{
        borderColor: 'transparent',
        backgroundColor: 'transparent',
    },
    dayBarItemTextDisabled:{
        color: '#7f7f7f',
    },
})